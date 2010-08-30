#ifdef HAVE_CONFIG_D_H
#include <libmaca/config_d.h>
#endif
#ifdef HAVE_SFST
#include <libmaca/morph/sfstanalyser.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libmaca/morph/morfeuszanalyser.h>
#endif

#include <libmaca/morph/mapanalyser.h>
#include <libmaca/morph/dispatchanalyser.h>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <libtoki/util/foreach.h>
#include <libtoki/util/confignode.h>
#include <libmaca/util/settings.h>
#include <libmaca/tagsetmanager.h>
#include <libmaca/tagsetparser.h>
#include <libmaca/conv/tagsetconverter.h>

#include <libmaca/io/plain.h>
#include <libmaca/io/xces.h>
#include <libmaca/io/xceswriter.h>
#include <libmaca/io/xcesreader.h>

#include <libmaca/util/debug.h>
#include <fstream>

int main(int argc, char** argv)
{
	std::string sfst, mdict, conv, cfg_analyser, readxces;
	std::string tagset_load;
	std::vector<std::string> plugins;
	using boost::program_options::value;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("analyser,a", value(&cfg_analyser),
			 "Analyser config file\n")
			("read,r", value(&readxces),
			 "Read a XCES file and hold\n")
			("plugin,P", value(&plugins),
			 "Additional plugins to load")
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

	boost::shared_ptr<Maca::Tagset> tagset(new Maca::Tagset);
	try {
		if (!tagset_load.empty()) {
			*tagset = Maca::get_named_tagset(tagset_load);
		}
		foreach (const std::string& s, plugins) {
			Maca::MorphAnalyser::load_plugin(s, false);
		}
		std::cerr << "Available analyser types: "
				<< boost::algorithm::join(Maca::MorphAnalyser::available_analyser_types(), " ") << "\n";
		if (!readxces.empty() && tagset) {
			std::ifstream ifs(readxces.c_str());
			if (!ifs.good()) {
				std::cerr << "File open error\n";
				return 8;
			}
			Maca::XcesReader xr(*tagset, ifs);
			std::vector<Maca::Chunk*> chunks;
			Maca::Chunk* ch = NULL;
			int sc = 0;
			int tc = 0;
			while ((ch = xr.get_next_chunk())) {
				chunks.push_back(ch);
				sc += ch->sentences().size();
				foreach (Maca::Sentence* s, ch->sentences()) {
					tc += s->size();
				}
			}
			std::cerr << "Read " << chunks.size() << " chunks, "
					<< sc << " sentences, " << tc << " tokens\n";
			std::string x;
			std::cin >> x;
			foreach (Maca::Chunk* c, chunks) {
				delete c;
			}
			std::cerr << "Deleted\n";
			std::cin >> x;
		}
	} catch (Maca::MacaError& e) {
		std::cerr << "Fatal error: " << e.info() << "\n";
		return 9;
	}

	std::cerr << "Nothing to do! Try --help.\n";
}

