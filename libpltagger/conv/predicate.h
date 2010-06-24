#ifndef LIBPLTAGGER_CONV_PREDICATE_H
#define LIBPLTAGGER_CONV_PREDICATE_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <unicode/unistr.h>

namespace PlTagger { namespace Conversion {

	/**
	 * Class for checking if a tag contains some value, has an attribute not
	 * set, or has a specific POS. See the constructor for details.
	 *
	 * Applying the predicate has teh effect of setting the appropriate value
	 * or POS so that the tag matches the predicate.
	 */
	class TagPredicate : public std::pair<idx_t, idx_t>
	{
	public:
		/**
		 * Create a tag predicate from a tagset and a string identifier of
		 * either a value, attribute or POS.
		 *
		 * If the name resolves to a value, the predicate will check for that
		 * value. If the name resolves to an attribute, the predicate will
		 * match tags with that attribute not set. Finally, if the name
		 * resolves to a POS, the predicate will check for that POS.
		 */
		TagPredicate(const std::string& name, const Tagset& tagset);

		/**
		 * Check the tag matches this predicate
		 */
		bool check(const Tag& tag) const;

		/**
		 * Check all tags of the token match this predicate
		 */
		bool token_match(const Token& t) const;

		/**
		 * Apply the predicate to the tag, modyfying it so it matches the predicate
		 */
		void apply(Tag& tag) const;

		/**
		 * Apply the predicate to all tags of the token
		 */
		void token_apply(Token& t) const;
	};

	class PosOrthPredicate : public std::pair<pos_idx_t, UnicodeString>
	{
	public:
		PosOrthPredicate();

		PosOrthPredicate(pos_idx_t pos, const UnicodeString& orth);

		bool check(const Token& token) const;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_PREDICATE_H
