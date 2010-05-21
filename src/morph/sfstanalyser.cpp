#include "sfstanalyser.h"

#include <sfst/interface.h>
#include <sfst/compact.h>
#include <cstdio>

namespace PlTagger {

	SfstAnalyser::SfstAnalyser(const std::string &filename)
	{
		FILE* f = fopen(filename.c_str(), "rb");
		ct_ = new CompactTransducer(f);
	}

	SfstAnalyser::~SfstAnalyser()
	{
	}

} /* end ns PlTagger */
