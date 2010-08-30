#include <libmaca/io/writer.h>
#include <libmaca/io/premorph.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>

#include <libtoki/sentencesplitter.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/util/foreach.h>
#include <libtoki/util/settings.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	std::vector<std::string> plugins;
	bool quiet = false, progress = false;
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
			("plugin,P", value(&plugins),
			 "Additional plugins to load")
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
				pp.parse_stream(std::cin);
				return 0;
			}

			tok.set_input_source(std::cin);
			Toki::SentenceSplitter sen(tok);
			boost::scoped_ptr<Maca::TokenWriter> writer;
			writer.reset(Maca::TokenWriter::create(output_format, std::cout, ma->tagset()));
			size_t tokens = 0, sentences = 0, sec_tokens = 0;
			clock_t start = clock();
			clock_t slice_start = start;
			while (sen.has_more()) {
				boost::scoped_ptr<Toki::Sentence> sentence(sen.get_next_sentence());
				assert(!sentence->empty());
				boost::scoped_ptr<Maca::Sentence> analysed(ma->process_dispose(sentence.get()));
				writer->write_sentence(*analysed);
				++sentences;
				tokens += analysed->size();
				if (progress) {
					sec_tokens += analysed->size();
					clock_t now_clock = clock();
					if (now_clock - CLOCKS_PER_SEC > slice_start) {
						double sec_elapsed = ((double)now_clock - slice_start) / CLOCKS_PER_SEC;
						double elapsed = ((double)now_clock - start) / CLOCKS_PER_SEC;
						double sec_rate = sec_tokens / sec_elapsed;
						double rate = tokens / elapsed;
						sec_tokens = 0;
						slice_start = now_clock;
						std::cerr << "\r" << "Processed " << sentences << " sentences, " << tokens << " tokens, "
								<< "rate " << sec_rate << " t/s, avg " << rate << " t/s    ";
					}

				}
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