#include "morph/sfstanalyser.h"

#include <iostream>

int main()
{
	PlTagger::SfstAnalyser fst("/home/ilor/semantic/tagger/fst/M.cfst");
	while (std::cin.good()) {
		std::string s;
		std::cin >> s;
		Toki::Token t(s.c_str(), "t", Toki::Whitespace::None);
		PlTagger::Token* tt = fst.process(t);
	}
}
