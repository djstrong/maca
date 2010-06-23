#ifndef LIBPLTAGGER_CONV_JOINRULE_H
#define LIBPLTAGGER_CONV_JOINRULE_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <libpltagger/conv/predicate.h>
#include <boost/function.hpp>

namespace PlTagger { namespace Conversion {

	class JoinRule
	{
	public:
		JoinRule(const Tagset& tagset);

		~JoinRule();

		Token* try_join(Token* t1, Token* t2) const;

		void set_token1_preconditions(const PosOrthPredicate& pre);

		void set_token1_preconditions(const std::string& pos, const UnicodeString& orth);

		void set_token2_preconditions(const PosOrthPredicate& pre);

		void set_token2_preconditions(const std::string& pos, const UnicodeString& orth);

		void set_copy_attrs(const std::vector<attribute_idx_t>& v);

		void add_copy_attr(const std::string& name);

		void add_postcondition(const TagPredicate& tp);

		void add_postcondition(const std::string& pred_string);

	private:
		const Tagset* tagset_;

		PosOrthPredicate pre1_;

		PosOrthPredicate pre2_;

		std::vector<attribute_idx_t> copy_t2_attrs_;

		std::vector<TagPredicate> post_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINRULE_H
