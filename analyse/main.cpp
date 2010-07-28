#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/debug.h>
#include <libpltagger/settings.h>
#include <libpltagger/tagsetparser.h>

#include <libpltagger/io/plain.h>
#include <libpltagger/io/xces.h>
#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/morph/creator.h>

#include <libtoki/foreach.h>
#include <libtoki/layertokenizer.h>
#include <libtoki/sentencesplitter.h>

#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	bool quiet = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("config,c", value(&config),
			 "Morphological analyser configuration file\n")
			("toki-config,t", value(&toki_config),
			 "Tokenizer configuration file (only used in some input modes)\n")
			("input-format,i", value(&input_format)->default_value("plain"),
			 "Input format\n")
			("output-format,o", value(&output_format)->default_value("plain"),
			 "Output format\n")
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
		if (!PlTagger::open_file_from_search_path(config, ifs)) {
			std::cerr << "Config file open error\n";
			return 8;
		}
		Toki::Config::Node cfg = Toki::Config::from_stream(ifs);
		boost::shared_ptr<PlTagger::MorphAnalyser> ma(PlTagger::create_analyser(cfg));
		const Toki::Config::Node& conf = toki_config.empty() ?
			Toki::Config::default_config() :
			Toki::Config::from_file(toki_config);
		Toki::LayerTokenizer tok(conf);
		tok.set_input_source(std::cin);
		Toki::SentenceSplitter sen(tok);
		PlTagger::PlainWriter pw(std::cout, ma->tagset());

		while (sen.has_more()) {
			std::vector<Toki::Token*> sentence = sen.get_next_sentence();
			assert(!sentence.empty());
			std::vector<PlTagger::Token*> analysed_sentence = ma->process_dispose(sentence);
			pw.write_sentence(analysed_sentence);
		}

	} else {
		std::cerr << "Usage: tagset-tool [OPTIONS] <tagset-file>\n";
		std::cerr << "See tagset-tool --help\n";
		return 1;
	}
	return 0;
}
