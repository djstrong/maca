#ifdef HAVE_SFST
#include "morph/sfstanalyser.h"
#endif

#include "morph/mapanalyser.h"

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "tagset.h"

#include "debug.h"
#include <fstream>

int main(int argc, char** argv)
{
	std::string sfst, mdict;
	std::string tagset_load, tagset_save;
	using boost::program_options::value;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
#ifdef HAVE_SFST
			("sfst-transducer,t", value(&sfst)->default_value("/home/ilor/semantic/tagger/fst/M.cfst"),
			 "SFST transducer file to use (compact format)")
#endif
			("m-dict-file,m", value(&mdict),
			 "M-style dictionary file to use (orth\\tlemma\\ttag)\n")
			("tagset-load,", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("tagset-save,", value(&tagset_save),
			 "Path to tagset ini file to save\n")
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

	if (!tagset_load.empty()) {
		PlTagger::Tagset tagset;
		std::ifstream ifs(tagset_load.c_str());
		try {
			tagset.load_from_stream(ifs);
		} catch (PlTagger::TagsetParseError& e) {
			std::cerr << e.info() << "\n";
			exit(1);
		}
		if (!tagset_save.empty()) {
			std::ofstream ofs(tagset_save.c_str());
			tagset.save_to_stream(ofs);
		}
		return 0;
	}

	boost::shared_ptr<PlTagger::MorphAnalyser> ma;

#ifdef HAVE_SFST
	if (!sfst.empty()) {
		ma.reset(new PlTagger::SfstAnalyser(sfst));
	} else
#endif
	if (!mdict.empty()) {
		PlTagger::HashMapAnalyser* hma = new PlTagger::HashMapAnalyser;
		ma.reset(hma);
		hma->load_m_dictionary(mdict);
		std::cout << "loading done\n";
	}

	if (ma) {
		while (std::cin.good()) {
			std::string s;
			std::cin >> s;
			Toki::Token t(s.c_str(), "t", Toki::Whitespace::None);
			PlTagger::Token* tt = ma->process(t);
			if (tt != NULL) {
				std::cout << PlTagger::token_string(*tt) << "\n";
			}
			delete tt;
		}
	}
}
