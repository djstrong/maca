#ifndef LIBPLTAGGER_MORFEUSZANALYSER_H
#define LIBPLTAGGER_MORFEUSZANALYSER_H

#include "morphanalyser.h"

namespace PlTagger {

	class MorfeuszAnalyser : public MorphAnalyser
	{
	public:
		MorfeuszAnalyser(const Tagset* tagset);
	};

} /* end ns Pltagger */

#endif // LIBPLTAGGER_MORFEUSZANALYSER_H
