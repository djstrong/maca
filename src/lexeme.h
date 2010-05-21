#ifndef LIBPLTAGGER_LEXEME_H
#define LIBPLTAGGER_LEXEME_H

#include <unicode/unistr.h>

namespace PlTagger {

	class Lexeme
	{
	public:
		Lexeme(const UnicodeString& lemma, const std::string& tag);

		const UnicodeString& lemma() const {
			return lemma_;
		}

		const std::string& tag() const {
			return tag_;
		}

	private:
		UnicodeString lemma_;

		std::string tag_;
	};
} /* end ns PlTagger */

#endif // LIBPLTAGGER_LEXEME_H
