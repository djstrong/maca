#include "lexeme.h"

namespace PlTagger {

	Lexeme::Lexeme()
		: lemma_(), tag_()
	{
	}

	Lexeme::Lexeme(const UnicodeString &lemma, const Tag &tag)
		: lemma_(lemma), tag_(tag)
	{
	}

} /* end ns PlTagger */
