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

	Lexeme Lexeme::create(const UnicodeString& lemma, const Tag& tag)
	{
		return Lexeme(lemma, tag);
	}

} /* end ns PlTagger */
