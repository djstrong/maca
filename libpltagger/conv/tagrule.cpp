#include <libpltagger/conv/tagrule.h>
#include <libtoki/foreach.h>
#include <iostream>

namespace PlTagger { namespace Conversion {

	TagRule::TagRule(const Tagset& tagset)
		: tagset_(&tagset)
	{
	}

	void TagRule::add_precondition(const TagPredicate &tp)
	{
		pre_.push_back(tp);
	}

	void TagRule::add_precondition(const std::string &pred_string)
	{
		pre_.push_back(TagPredicate(pred_string, *tagset_));
	}

	void TagRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	void TagRule::add_postcondition(const std::string &pred_string)
	{
		post_.push_back(TagPredicate(pred_string, *tagset_));
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

	void TagRule::apply(Token &token) const
	{
		foreach (Lexeme& lex, token.lexemes()) {
			apply(lex.tag());
		}
	}

	Tag TagRule::apply_copy(const Tag &tag) const
	{
		Tag tag2(tag);
		apply(tag2);
		return tag2;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
