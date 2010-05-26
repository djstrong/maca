#include "sfstanalyser.h"

#include <sfst/interface.h>
#include <sfst/compact.h>
#include <cstdio>
#include <libtoki/foreach.h>
#include <sstream>

namespace PlTagger {

	SfstAnalyser::SfstAnalyser(const std::string &filename)
	{
		FILE* f = fopen(filename.c_str(), "rb");
		ct_ = new CompactTransducer(f);
	}

	SfstAnalyser::~SfstAnalyser()
	{
		delete ct_;
	}

	Token* SfstAnalyser::process(const Toki::Token &t)
	{
		std::vector< CAnalysis > a;
		std::string s = t.orth_utf8();
		ct_->analyze_string(const_cast<char*>(s.c_str()), a);
		foreach (CAnalysis& ca, a) {
			std::stringstream ss;
			foreach (unsigned int c, ca) {
				ss << ct_->alphabet.code2symbol(c);
			}
			std::cout << s << "\t" << ct_->print_analysis(ca) << "\n";
		}
		return NULL;
	}

} /* end ns PlTagger */
