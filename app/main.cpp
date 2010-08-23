#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#ifdef HAVE_SFST
#include <libpltagger/morph/sfstanalyser.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libpltagger/morph/morfeuszanalyser.h>
#endif

#include <libpltagger/morph/mapanalyser.h>
#include <libpltagger/morph/dispatchanalyser.h>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <libtoki/util/foreach.h>
#include <libtoki/util/confignode.h>
#include <libpltagger/util/settings.h>
#include <libpltagger/tagsetmanager.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/conv/tagsetconverter.h>

#include <libpltagger/io/plain.h>
#include <libpltagger/io/xces.h>
#include <libpltagger/io/xceswriter.h>

#include <libpltagger/util/debug.h>
#include <fstream>

int main(int argc, char** argv)
{
	std::string sfst, mdict, conv, cfg_analyser;
	std::string tagset_load;
	std::vector<std::string> plugins;
	using boost::program_options::value;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("analyser,a", value(&cfg_analyser),
			 "Analyser config file\n")
			("plugin,p", value(&plugins),
			 "Plugins to load")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

	boost::shared_ptr<PlTagger::Tagset> tagset(new PlTagger::Tagset);

	if (!tagset_load.empty()) {
		std::ifstream ifs(tagset_load.c_str());
		if (ifs.good()) {
			try {
				*tagset = PlTagger::TagsetParser::load_ini(ifs);
			} catch (PlTagger::TagsetParseError& e) {
				std::cerr << e.info() << "\n";
				exit(1);
			}
		} else {
			std::cerr << "tagset file open error\n";
			return 7;
		}
	}
	foreach (const std::string& s, plugins) {
		PlTagger::MorphAnalyser::load_plugin(s, false);
	}
	std::cerr << "Available analyser types: "
			<< boost::algorithm::join(PlTagger::MorphAnalyser::available_analyser_types(), " ") << "\n";

	std::cerr << "Nothing to do! Try --help.\n";
}
