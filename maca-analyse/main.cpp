#include <libmaca/io/writer.h>
#include <libmaca/io/premorph.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>
#include <libmaca/util/tokentimer.h>

#include <libtoki/sentencesplitter.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/util/foreach.h>
#include <libtoki/util/settings.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fstream>
#include <omp.h>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	std::vector<std::string> plugins;
	int threads = 4;
	bool quiet = false, progress = false, split_chunks = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Maca::TokenWriter::available_writer_types(), " ");

	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("config,c", value(&config),
			 "Morphological analyser configuration file\n")
			("toki-config,t", value(&toki_config),
			 "Tokenizer configuration file. "
			 "Overrides config value, only used in some input modes.\n")
			("input-format,i", value(&input_format)->default_value("plain"),
			 "Input format, any of: plain premorph\n")
			("output-format,o", value(&output_format)->default_value("plain"),
			 writers_help.c_str())
			("split,s", value(&split_chunks)->zero_tokens(),
			 "Split output into chunks on many-newline tokens")
			("plugin,P", value(&plugins),
			 "Additional plugins to load")
			("threads,T", value(&threads),
			 "Threads to use")
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset\n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("config", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	foreach (const std::string& s, plugins) {
		Maca::MorphAnalyser::load_plugin(s, false);
	}

	if (vm.count("help")) {
		std::cout << desc << "\n";
		std::cout << "Available analyser types: ";
		std::cout << boost::algorithm::join(Maca::MorphAnalyser::available_analyser_types(), " ") << "\n";
		return 1;
	}
	Toki::Path::Instance().set_verbose(!quiet);
	Maca::Path::Instance().set_verbose(!quiet);

	if (!config.empty()) {
		try {
			Maca::Config::Node cfg = Maca::get_named_config(config);
			boost::shared_ptr<Maca::MorphAnalyser> ma(new Maca::DispatchAnalyser(cfg));

			if (toki_config.empty()) {
				toki_config = cfg.get("general.toki-config", "");
			}
			const Toki::Config::Node& conf = toki_config.empty() ?
				Toki::default_config() :
				Toki::get_named_config(toki_config);
			Toki::LayerTokenizer tok(conf);

			if (input_format == "premorph" || input_format == "pre") {
				Maca::PremorphProcessor pp(std::cout, tok, *ma);
				pp.set_stats(progress);
				pp.parse_stream(std::cin);
				return 0;
			}

			tok.set_input_source(std::cin);
			Toki::SentenceSplitter sen(tok);
			boost::scoped_ptr<Maca::TokenWriter> writer;
			writer.reset(Maca::TokenWriter::create(output_format, std::cout, ma->tagset()));
			Maca::TokenTimer timer;
			boost::scoped_ptr<Maca::Chunk> ch(new Maca::Chunk);

			omp_set_num_threads(threads);
			std::vector< boost::shared_ptr<Maca::MorphAnalyser> > analysers;
			analysers.push_back(ma);
			for (int i = 1; i < threads; ++i) {
				std::cerr << "C";
				analysers.push_back(boost::shared_ptr<Maca::MorphAnalyser>(ma->clone()));
			}

			std::vector<Toki::Sentence*> sss;
			while (sen.has_more()) {
				sss.push_back(sen.get_next_sentence());
			}
			std::cerr << "phase 2\n";
			ch->sentences().resize(sss.size());
#pragma omp parallel for
			for (size_t i = 0; i < sss.size(); ++i) {
				ch->sentences()[i] = analysers[omp_get_thread_num()]->process_dispose(sss[i]);
				timer.count_sentence(*ch->sentences()[i]);
				if (omp_get_thread_num() == 0 && progress) {
					timer.check_slice();
				}
			}

			/*
			{
				boost::scoped_ptr<Toki::Sentence> sentence(sen.get_next_sentence());
				assert(!sentence->empty());
				std::auto_ptr<Maca::Sentence> analysed(new Maca::Sentence); //(ma->process_dispose(sentence.get()));
				std::vector< std::vector< Maca::Token* > > results;
				results.resize(sentence->size());
				for (size_t i = 0; i < sentence->size(); ++i) {
					analysers[omp_get_thread_num()]->process(*(sentence->tokens()[i]), results[i]);
				}
				for (size_t i = 0; i < results.size(); ++i) {
					foreach (Maca::Token* t, results[i]) {
						analysed->append(t);
					}
				}
				timer.count_sentence(*analysed);
				if (split_chunks) {
					if (analysed->tokens()[0]->wa() == Toki::Whitespace::ManyNewlines) {
						if (!ch->sentences().empty()) {
							writer->write_chunk(*ch);
							ch.reset(new Maca::Chunk());
							if (progress) {
								timer.check_slice();
							}
						}
					}
					ch->append(analysed.release());
				} else {
					writer->write_sentence(*analysed);
					if (progress) {
						timer.check_slice();
					}
				}
			}
			*/
			if (!ch->sentences().empty()) {
				writer->write_chunk(*ch);
			}
			if (progress) {
				timer.stats();
			}
		} catch (Maca::MacaError& e) {
			std::cerr << "Maca Error: " << e.info() << "\n";
			return 4;
		} catch (Toki::TokenizerLibError& e) {
			std::cerr << "Tokenizer Error: " << e.info() << "\n";
			return 6;
		}
	} else {
		std::cerr << "Usage: analyse -c CONFIG [OPTIONS]\n";
		std::cerr << "See analyse --help\n";
		return 1;
	}
	return 0;
}
