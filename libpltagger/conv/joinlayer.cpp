#include <libpltagger/conv/joinlayer.h>
#include <libpltagger/tagsetmanager.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	JoinLayer::JoinLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), buf_(NULL)
	{
	}

	JoinLayer::JoinLayer(const Toki::Config::Node& cfg)
		: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset"))), buf_(NULL)
	{
		append_rule(cfg);
	}


	void JoinLayer::append_rule(const JoinRule &rule)
	{
		rules_.push_back(rule);
	}

	void JoinLayer::append_rule(const Toki::Config::Node& cfg)
	{
		JoinRule jr(tagset_from());
		std::string pos1, pos2;
		UnicodeString orth1, orth2;
		foreach (const Toki::Config::Node::value_type &v, cfg) {
			if (v.first == "t1_pos") {
				pos1 = v.second.data();
			} else if (v.first == "t2_pos") {
				pos2 = v.second.data();
			} else if (v.first == "t1_orth") {
				orth1 = UnicodeString::fromUTF8(v.second.data());
			} else if (v.first == "t2_orth") {
				orth2 = UnicodeString::fromUTF8(v.second.data());
			} else if (v.first == "post") {
				jr.add_postcondition(v.second.data());
			} else if (v.first == "copy_attr") {
				jr.add_copy_attr(v.second.data());
			}
		}
		jr.set_token1_preconditions(pos1, orth1);
		jr.set_token2_preconditions(pos2, orth2);
		append_rule(jr);
	}

	Token* JoinLayer::get_next_token()
	{
		if (buf_ == NULL) {
			buf_ = source()->get_next_token();
		}
		Token* t = source()->get_next_token();
		if (t != NULL && t->wa() == Toki::Whitespace::None) {
			foreach (JoinRule& rule, rules_) {
				Token* joined = rule.try_join(buf_, t);
				if (joined != NULL) {
					buf_ = NULL;
					return joined;
				}
			}
		}
		std::swap(t, buf_);
		return t;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
