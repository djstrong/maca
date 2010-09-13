#include <libmaca/conv/joinlayer.h>
#include <libmaca/tagsetmanager.h>
#include <libtoki/util/foreach.h>

namespace Maca { namespace Conversion {

	JoinLayer::JoinLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), buf_(NULL)
	{
	}

	JoinLayer::JoinLayer(const Config::Node& cfg)
		: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset")))
		, buf_(NULL)
	{
		append_rule(cfg);
	}

	JoinLayer::~JoinLayer()
	{
		delete buf_;
	}

	JoinLayer* JoinLayer::clone() const
	{
		JoinLayer* copy = new JoinLayer(tagset());
		copy->rules_ = rules_;
		if (buf_ != NULL) {
			copy->buf_ = buf_->clone();
		}
		return copy;
	}

	void JoinLayer::append_rule(const JoinRule &rule)
	{
		if (rule.tagset().id() != tagset().id()) {
			throw TagsetMismatch("appending join rule", tagset(),
					rule.tagset());
		}
		rules_.push_back(rule);
	}

	void JoinLayer::append_rule(const Config::Node& cfg)
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
					buf_ = joined;
					t = source()->get_next_token();
					if (t == NULL || t->wa() != Toki::Whitespace::None) {
						break;
					}
				}
			}
		}
		std::swap(t, buf_);
		return t;
	}

} /* end ns Conversion */ } /* end ns Maca */
