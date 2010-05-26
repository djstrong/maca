#ifndef LIBPLTAGGER_LEXEME_H
#define LIBPLTAGGER_LEXEME_H

#include <unicode/unistr.h>

#include "tag.h"

namespace PlTagger {

	class Lexeme
	{
	public:
		Lexeme(const UnicodeString& lemma, const Tag& tag);

		const UnicodeString& lemma() const {
			return lemma_;
		}

		const Tag& tag() const {
			return tag_;
		}

	private:
		UnicodeString lemma_;

		Tag tag_;
	};
} /* end ns PlTagger */

#endif // LIBPLTAGGER_LEXEME_H
