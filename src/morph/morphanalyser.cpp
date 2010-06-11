#include "morphanalyser.h"

#include <morfeusz.h>

namespace PlTagger {

	MorphAnalyser::MorphAnalyser(const Tagset *tagset)
		: tagset_(tagset)
	{
	}

	MorphAnalyser::~MorphAnalyser()
	{
	}

} /* end ns PlTagger */
