#include <libpltagger/conv/joinrule.h>
#include <libtoki/foreach.h>
#include <libpltagger/tagsetmanager.h>
#include <libpltagger/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>
namespace PlTagger { namespace Conversion {

	JoinRule::JoinRule(const Tagset& tagset)
		: tagset_(&tagset)
	{
	}

	JoinRule::JoinRule(const Config::Node& cfg)
		: tagset_(&get_named_tagset(cfg.get<std::string>("tagset")))
	{
		std::string pos1, pos2;
		UnicodeString orth1, orth2;
		foreach (const Config::Node::value_type &v, cfg) {
			if (v.first == "t1_pos") {
				pos1 = v.second.data();
			} else if (v.first == "t2_pos") {
				pos2 = v.second.data();
			} else if (v.first == "t1_orth") {
				orth1 = UnicodeString::fromUTF8(v.second.data());
			} else if (v.first == "t2_orth") {
				orth2 = UnicodeString::fromUTF8(v.second.data());
			} else if (v.first == "post") {
				add_postcondition(v.second.data());
			} else if (v.first == "copy_attr") {
				append_copy_attrs(v.second.data());
			}
		}
		set_token1_preconditions(pos1, orth1);
		set_token2_preconditions(pos2, orth2);
	}

	void JoinRule::set_token1_preconditions(const PosOrthPredicate &pre)
	{
		pre1_ = pre;
	}

	void JoinRule::set_token1_preconditions(const std::string& pos, const UnicodeString& orth)
	{
		pos_idx_t p = tagset_->pos_dictionary().get_id(pos);
		if (!tagset_->pos_dictionary().is_id_valid(p)) {
			p = static_cast<pos_idx_t>(-1);
		}
		pre1_ = PosOrthPredicate(p, orth);
	}

	void JoinRule::set_token2_preconditions(const PosOrthPredicate &pre)
	{
		pre1_ = pre;
	}

	void JoinRule::set_token2_preconditions(const std::string& pos, const UnicodeString& orth)
	{
		pos_idx_t p = tagset_->pos_dictionary().get_id(pos);
		if (!tagset_->pos_dictionary().is_id_valid(p)) {
			p = static_cast<pos_idx_t>(-1);
		}
		pre2_ = PosOrthPredicate(p, orth);
	}

	void JoinRule::set_copy_attrs(const std::vector<attribute_idx_t> &v)
	{
		copy_t2_attrs_ = v;
	}

	void JoinRule::append_copy_attrs(const std::string& names)
	{
		append_attribute_list(copy_t2_attrs_, tagset(), names);
	}

	void JoinRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	void JoinRule::add_postcondition(const std::string& pred_string)
	{
		std::vector<std::string> srv;
		boost::algorithm::split(srv, pred_string, boost::is_any_of(": "));
		foreach (const std::string& sr, srv) {
			if (!sr.empty()) {
				post_.push_back(TagPredicate(sr, *tagset_));
			}
		}
	}

	Token* JoinRule::try_join(Token* t1, Token* t2) const
	{
		if (pre1_.check(*t1) && pre2_.check(*t2)) {
			t1->set_orth(t1->orth() + t2->orth());
			copy_attributes(*t2, copy_t2_attrs_, *t1);
			foreach (const TagPredicate& p, post_) {
				p.token_apply(*t1);
			}
			delete t2;
			return t1;
		} else {
			return NULL;
		}
	}


} /* end ns Conversion */ } /* end ns PlTagger */
