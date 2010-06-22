#ifndef LIBPLTAGGER_CONV_PREDICATE_H
#define LIBPLTAGGER_CONV_PREDICATE_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <unicode/unistr.h>

namespace PlTagger { namespace Conversion {

	class TagPredicate : public std::pair<idx_t, idx_t>
	{
	public:
		TagPredicate(const std::string& name, const Tagset& tagset);

		bool check(const Tag& tag) const;

		void apply(Tag& tag) const;
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
