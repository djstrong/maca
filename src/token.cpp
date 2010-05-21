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

} /* end ns PlTagger */
