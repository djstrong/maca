#ifdef HAVE_SFST
#include "morph/sfstanalyser.h"
#endif

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include "debug.h"

int main(int argc, char** argv)
{
	std::string sfst;
	using boost::program_options::value;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
#ifdef HAVE_SFST
			("sfst-transducer,t", value(&sfst)->default_value("/home/ilor/semantic/tagger/fst/M.cfst"),
			 "SFST transducer file to use (compact format)")
#endif
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

#ifdef HAVE_SFST
	if (!sfst.empty()) {
		PlTagger::SfstAnalyser fst(sfst);
		while (std::cin.good()) {
			std::string s;
			std::cin >> s;
			Toki::Token t(s.c_str(), "t", Toki::Whitespace::None);
			PlTagger::Token* tt = fst.process(t);
			std::cout << PlTagger::token_string(*tt) << "\n";
			delete tt;
		}
	}
#endif
}
