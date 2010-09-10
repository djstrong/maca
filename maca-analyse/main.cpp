#include <libmaca/io/writer.h>
#include <libmaca/io/premorph.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>
#include <libmaca/util/sentenceanalyser.h>
#include <libmaca/util/tokentimer.h>

#include <libtoki/sentencesplitter.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/util/foreach.h>
#include <libtoki/util/settings.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <omp.h>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	std::vector<std::string> plugins;
	int threads = 0;
	bool quiet = false, progress = false, split_chunks = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Maca::TokenWriter::available_writer_types_help(), " ");

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
			boost::shared_ptr<Maca::SentenceAnalyser> sa;
			if (toki_config.empty()) {
				sa = Maca::SentenceAnalyser::create_from_named_config(config);
			} else {
				sa = Maca::SentenceAnalyser::create_from_named_config(config, toki_config);
			}

			if (input_format == "premorph" || input_format == "pre") {
				Maca::PremorphProcessor pp(std::cout, sa);
				pp.set_stats(progress);
				pp.parse_stream(std::cin);
				return 0;
			}

			sa->set_input_source(std::cin);
			//Maca::PremorphReader pr(std::cin, sa);
			boost::scoped_ptr<Maca::TokenWriter> writer;
			writer.reset(Maca::TokenWriter::create(output_format, std::cout, sa->tagset()));
			Maca::TokenTimer& timer = Maca::global_timer();
			timer.register_signal_handler();
			boost::scoped_ptr<Maca::Chunk> ch(new Maca::Chunk);

			while (Maca::Sentence* sentence = sa->get_next_sentence()) {
				assert(!sentence->empty());
				timer.count_sentence(*sentence);
				if (split_chunks) {
					if (sentence->first_token()->wa() == Toki::Whitespace::ManyNewlines) {
						if (!ch->sentences().empty()) {
							writer->write_chunk(*ch);
							ch.reset(new Maca::Chunk());
							if (progress) {
								timer.check_slice();
							}
						}
					}
					ch->append(sentence);
				} else {
					writer->write_sentence(*sentence);
					if (progress) {
						timer.check_slice();
					}
				}
			}
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
