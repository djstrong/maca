#ifndef LIBMACA_SENTENCE_H
#define LIBMACA_SENTENCE_H

#include <libmaca/token.h>

#include <libtoki/sentence.h>

namespace Maca {

	/// typedef for a sentence of Maca tokens based on the Toki sentence
	/// template.
	typedef Toki::SentenceTemplate<Token> Sentence;

} /* end ns Maca */

#endif // LIBMACA_SENTENCE_H
