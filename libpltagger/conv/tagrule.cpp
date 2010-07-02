#include <libpltagger/conv/tagrule.h>
#include <libtoki/foreach.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

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
		std::vector<std::string> srv;
		boost::algorithm::split(srv, pred_string, boost::is_any_of(": "));
		foreach (const std::string& sr, srv) {
			if (!sr.empty()) {
				pre_.push_back(TagPredicate(sr, *tagset_));
			}
		}
	}

	void TagRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	void TagRule::add_postcondition(const std::string &pred_string)
	{
		std::vector<std::string> srv;
		boost::algorithm::split(srv, pred_string, boost::is_any_of(": "));
		foreach (const std::string& sr, srv) {
			if (!sr.empty()) {
				post_.push_back(TagPredicate(sr, *tagset_));
			}
		}
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
		token.remove_duplicate_lexemes();
	}

	Tag TagRule::apply_copy(const Tag &tag) const
	{
		Tag tag2(tag);
		apply(tag2);
		return tag2;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
