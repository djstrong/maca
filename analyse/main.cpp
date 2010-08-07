#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/debug.h>
#include <libpltagger/settings.h>
#include <libpltagger/tagsetparser.h>

#include <libpltagger/io/plain.h>
#include <libpltagger/io/premorph.h>
#include <libpltagger/io/xces.h>
#include <libpltagger/morph/dispatchanalyser.h>

#include <libtoki/foreach.h>
#include <libtoki/layertokenizer.h>
#include <libtoki/sentencesplitter.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	bool quiet = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(PlTagger::TokenWriter::available_writer_types(), " ");

	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("config,c", value(&config),
			 "Morphological analyser configuration file\n")
			("toki-config,t", value(&toki_config),
			 "Tokenizer configuration file. "
			 "Overrides tagger config value, only used in some input modes.\n")
			("input-format,i", value(&input_format)->default_value("plain"),
			 "Input format, any of: plain premorph\n")
			("output-format,o", value(&output_format)->default_value("plain"),
			 writers_help.c_str())
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset\n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("tagset", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}
	if (!config.empty()) {
		std::ifstream ifs;
		std::string fn = PlTagger::find_file_in_search_path(config);
		if (fn.empty()) {
			std::cerr << "Config file open error for " << config << "\n";
			return 8;
		}
		ifs.open(fn.c_str());
		std::cerr << "Loading tagger configuration from " << fn << "\n";
		Toki::Config::Node cfg = Toki::Config::from_stream(ifs);
		boost::shared_ptr<PlTagger::MorphAnalyser> ma(new PlTagger::DispatchAnalyser(cfg));

		if (toki_config.empty()) {
			toki_config = cfg.get("general.toki-config", "");
		}
		const Toki::Config::Node& conf = toki_config.empty() ?
			Toki::Config::default_config() :
			Toki::Config::get_library_config(toki_config);
		Toki::LayerTokenizer tok(conf);

		if (input_format == "premorph" || input_format == "pre") {
			PlTagger::PremorphProcessor pp(std::cout, tok, *ma);
			pp.parse_stream(std::cin);
			return 0;
		}

		tok.set_input_source(std::cin);
		Toki::SentenceSplitter sen(tok);
		boost::scoped_ptr<PlTagger::TokenWriter> writer;
		writer.reset(PlTagger::TokenWriter::create(output_format, std::cout, ma->tagset()));

		while (sen.has_more()) {
			std::vector<Toki::Token*> sentence = sen.get_next_sentence();
			assert(!sentence.empty());
			std::vector<PlTagger::Token*> analysed_sentence = ma->process_dispose(sentence);
			writer->write_sentence(analysed_sentence);
			foreach (PlTagger::Token* t, analysed_sentence) {
				delete t;
			}
		}

	} else {
		std::cerr << "Usage: tagset-tool [OPTIONS] <tagset-file>\n";
		std::cerr << "See tagset-tool --help\n";
		return 1;
	}
	return 0;
}
