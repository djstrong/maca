#include "token.h"
#include <sstream>

namespace PlTagger {

	Token::Token()
		: orth_(), wa_(), lexemes_()
	{
	}

	Token::Token(const UnicodeString &orth, Toki::Whitespace::Enum wa)
		: orth_(orth), wa_(wa), lexemes_()
	{
	}

	Token::Token(const Toki::Token &tok)
		: orth_(tok.orth()), wa_(tok.preceeding_whitespace()), lexemes_()
	{
	}

} /* end ns PlTagger */
