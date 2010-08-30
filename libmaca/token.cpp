#include <libmaca/token.h>
#include <sstream>
#include <libtoki/util/foreach.h>

namespace Maca {

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

	Token* Token::clone() const
	{
		Token* t = new Token(*this);
		return t;
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

	bool Token::operator ==(const Token& other) const
	{
		return orth_ == other.orth_ && wa_ == other.wa_
				&& lexemes_ == other.lexemes_;
	}

	bool Token::check_duplicate_lexemes() const
	{
		std::set<Lexeme> s(lexemes_.begin(), lexemes_.end());
		return s.size() != lexemes_.size();
	}

	bool Token::remove_duplicate_lexemes()
	{
		size_t old_size = lexemes_.size();
		std::sort(lexemes_.begin(), lexemes_.end());
		lexemes_.erase(std::unique(lexemes_.begin(), lexemes_.end()), lexemes_.end());
		return old_size != lexemes_.size();
	}

	bool Token::orth_pos_match(pos_idx_t pos, const UnicodeString &orth) const
	{
		if (orth.length() > 0) {
			if (orth.caseCompare(orth_, 0) != 0) return false;
		}
		if (pos != static_cast<pos_idx_t>(-1)) {
			foreach (const Lexeme& lex, lexemes_) {
				if (lex.tag().pos_id() != pos) return false;
			}
		}
		return true;
	}

} /* end ns Maca */
