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

	void Token::make_ign(const Tagset& tagset)
	{
		lexemes_.clear();
		add_ign(tagset);
	}

	void Token::add_ign(const Tagset &tagset)
	{
		Lexeme lex(orth_, tagset.make_ign_tag());
		lexemes_.push_back(lex);
	}

} /* end ns PlTagger */
