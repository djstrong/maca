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
		if (rule.tagset().id() != tagset().id()) {
			throw TagsetMismatch("appending join rule", tagset(), rule.tagset());
		}
		rules_.push_back(rule);
	}

	void JoinLayer::append_rule(const Toki::Config::Node& cfg)
	{
		JoinRule jr(cfg);
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
