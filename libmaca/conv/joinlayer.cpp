#include <libmaca/conv/joinlayer.h>
#include <libcorpus2/tagsetmanager.h>
#include <libpwrutils/foreach.h>

namespace Maca {
namespace Conversion {

JoinLayer::JoinLayer(const Corpus2::Tagset& tagset)
	: OneTagsetLayer(tagset), buf_(NULL)
{
}

JoinLayer::JoinLayer(const Config::Node& cfg)
	: OneTagsetLayer(Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
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
	require_matching_tagsets(rule, *this, "appending join rule");
	rules_.push_back(rule);
}

void JoinLayer::append_rule(const Config::Node& cfg)
{
	JoinRule jr(cfg);
	append_rule(jr);
}

Corpus2::Token* JoinLayer::get_next_token()
{
	if (buf_ == NULL) {
		buf_ = source()->get_next_token();
	}
	Corpus2::Token* t = source()->get_next_token();
	if (t != NULL && t->wa() == PwrNlp::Whitespace::None) {
		foreach (JoinRule& rule, rules_) {
			Corpus2::Token* joined = rule.try_join(buf_, t);
			if (joined != NULL) {
				buf_ = joined;
				t = source()->get_next_token();
				if (t == NULL || t->wa() != PwrNlp::Whitespace::None) {
					break;
				}
			}
		}
	}
	std::swap(t, buf_);
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
