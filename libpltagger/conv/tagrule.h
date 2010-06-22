#ifndef LIBPLTAGGER_CONV_TAGRULE_H
#define LIBPLTAGGER_CONV_TAGRULE_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <libpltagger/conv/predicate.h>
#include <boost/function.hpp>

namespace PlTagger { namespace Conversion {

	class TagRule
	{
	public:
		TagRule();

		void add_precondition(const TagPredicate& tp);

		void add_postcondition(const TagPredicate& tp);

		void apply(Tag& tag) const;

		Tag apply_copy(const Tag& tag) const;

	private:
		std::vector<TagPredicate> pre_;

		std::vector<TagPredicate> post_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_TAGRULE_H
