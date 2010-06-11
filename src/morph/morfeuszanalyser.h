#ifndef LIBPLTAGGER_MORFEUSZANALYSER_H
#define LIBPLTAGGER_MORFEUSZANALYSER_H

#include "morphanalyser.h"

#include <morfeusz.h>

namespace PlTagger {

	class MorfeuszAnalyser : public MorphAnalyser
	{
	public:
		MorfeuszAnalyser(const Tagset* tagset);

		void process_functional(const Toki::Token &t, boost::function<void(Token *)>sink);

		Token* make_token(const Toki::Token& t, InterpMorf* im) const;
	};

} /* end ns Pltagger */

#endif // LIBPLTAGGER_MORFEUSZANALYSER_H
