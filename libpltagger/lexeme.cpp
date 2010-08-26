#include <libpltagger/lexeme.h>

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

	bool Lexeme::is_null() const
	{
		return lemma().length() == 0 || !tag().has_valid_tagset();
	}

	bool Lexeme::operator<(const Lexeme& other) const
	{
		return lemma_ < other.lemma_ || (lemma_ == other.lemma_ && tag_ < other.tag_);
	}

	bool Lexeme::operator==(const Lexeme& other) const
	{
		return lemma_ == other.lemma_ && tag_ == other.tag_;
	}

} /* end ns PlTagger */
