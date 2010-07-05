#ifndef LIBPLTAGGER_LEXEME_H
#define LIBPLTAGGER_LEXEME_H

#include <unicode/unistr.h>

#include <libpltagger/tag.h>
#include <libtoki/util.h>

namespace PlTagger {

	class Lexeme
		: boost::equality_comparable<Lexeme>, boost::less_than_comparable<Lexeme>
	{
	public:
		Lexeme();

		Lexeme(const UnicodeString& lemma, const Tag& tag);

		static Lexeme create(const UnicodeString& lemma, const Tag& tag);

		const UnicodeString& lemma() const {
			return lemma_;
		}

		void set_lemma(const UnicodeString& l) {
			lemma_ = l;
		}

		const std::string lemma_utf8() const {
			return Toki::Util::to_utf8(lemma_);
		}

		const Tag& tag() const {
			return tag_;
		}

		Tag& tag() {
			return tag_;
		}

		bool is_null() const;

		/**
		 * Lexeme comparison. Lexemes are ordered by lemma and then by tag, see
		 * Tag::operator< . Boost template magic provides other comparison ops.
		 */
		bool operator<(const Lexeme& other) const;

		/**
		 * Lexeme equality. Lemma and tag must compare equal. Boost template
		 * magic provides operator!=.
		 */
		bool operator==(const Lexeme& other) const;

	private:
		/// The lemma -- basic form
		UnicodeString lemma_;

		/// The tag
		Tag tag_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_LEXEME_H
