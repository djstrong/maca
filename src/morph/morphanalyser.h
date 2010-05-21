#ifndef LIBPLTAGGER_MORPHANALYSER_H
#define LIBPLTAGGER_MORPHANALYSER_H

#include <libtoki/token.h>
#include "token.h"

namespace PlTagger {

	class MorphAnalyser
	{
	public:
		MorphAnalyser();

		virtual ~MorphAnalyser();

		virtual Token* process(const Toki::Token& t) = 0;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORPHANALYSER_H
