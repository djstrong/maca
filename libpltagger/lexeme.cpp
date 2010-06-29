#include <libpltagger/lexeme.h>
#include <libpltagger/tagsetmanager.h>

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

	bool Lexeme::not_null() const
	{
		return lemma_.length() > 0 && TagsetManagerSingleton::Instance().get_cache_entry(tag_.tagset_id());
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
