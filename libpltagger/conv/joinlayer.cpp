#include <libpltagger/conv/joinlayer.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	JoinLayer::JoinLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), buf_(NULL)
	{
	}

	void JoinLayer::append_rule(const JoinRule &rule)
	{
		rules_.push_back(rule);
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
