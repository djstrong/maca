#include "lexeme.h"

namespace PlTagger {

	Lexeme::Lexeme(const UnicodeString &lemma, const std::string &tag)
		: lemma_(lemma), tag_(tag)
	{
	}

} /* end ns PlTagger */
