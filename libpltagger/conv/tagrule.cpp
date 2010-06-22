#include <libpltagger/conv/tagrule.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TagRule::TagRule()
	{
	}

	void TagRule::add_precondition(const TagPredicate &tp)
	{
		pre_.push_back(tp);
	}

	void TagRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	void TagRule::apply(Tag &tag) const
	{
		foreach (const TagPredicate& tp, pre_) {
			if (!tp.check(tag)) return;
		}
		foreach (const TagPredicate& tp, post_) {
			tp.apply(tag);
		}
	}

	Tag TagRule::apply_copy(const Tag &tag) const
	{
		Tag tag2(tag);
		apply(tag2);
		return tag2;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
