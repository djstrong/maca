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
		JoinRule();

		~JoinRule();

		Token* try_join(const Token& t1, const Token& t2) const;

		void set_preconditions(const PosOrthPredicate& pre1, const PosOrthPredicate& pre2);

		void set_copy_attrs(const std::vector<attribute_idx_t>& v);

		void add_postcondition(const TagPredicate& tp);

	private:
		PosOrthPredicate pre1_;

		PosOrthPredicate pre2_;

		std::vector<attribute_idx_t> copy_t2_attrs_;

		std::vector<TagPredicate> post_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINRULE_H
