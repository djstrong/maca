#ifndef LIBMACA_LEXEME_H
#define LIBMACA_LEXEME_H

#include <unicode/unistr.h>
#include <libmaca/tag.h>
#include <libtoki/util/util.h>

#include <boost/flyweight.hpp>

U_NAMESPACE_BEGIN
inline size_t hash_value(const UnicodeString& u)
{
	return u.hashCode();
}
U_NAMESPACE_END


namespace Maca {

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

		void set_tag(const Tag& tag) {
			tag_ = tag;
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
		//boost::flyweight<UnicodeString> lemma_;
		UnicodeString lemma_;

		/// The tag
		//boost::flyweight<Tag> tag_;
		Tag tag_;
	};

} /* end ns Maca */

#endif // LIBMACA_LEXEME_H
