#ifndef LIBPLTAGGER_SENTENCE_H
#define LIBPLTAGGER_SENTENCE_H

#include <libpltagger/token.h>

#include <libtoki/sentence.h>

namespace PlTagger {

	/// typedef for a sentence of PlTagger tokens based on the Toki sentence
	/// template.
	typedef Toki::SentenceTemplate<Token> Sentence;

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SENTENCE_H
