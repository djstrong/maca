#include <libmaca/conv/tagrulelayer.h>
#include <libpwrutils/foreach.h>
#include <libcorpus2/tagsetmanager.h>

namespace Maca {
namespace Conversion {

TagRuleLayer::TagRuleLayer(const Tagset& tagset)
	: OneTagsetLayer(tagset), rules_()
{
}

TagRuleLayer::TagRuleLayer(const Config::Node& cfg)
	: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset")))
	, rules_()
{
	append_rule(cfg);
}

TagRuleLayer* TagRuleLayer::clone() const
{
	return new TagRuleLayer(*this);
}

void TagRuleLayer::append_rule(const TagRule &tr)
{
	rules_.push_back(tr);
}

void TagRuleLayer::append_rule(const Config::Node& cfg)
{
	TagRule tr(tagset_from());
	foreach (const Config::Node::value_type &v, cfg) {
		if (v.first == "pre") {
			tr.add_precondition(v.second.data());
		} else if (v.first == "post") {
			tr.add_postcondition(v.second.data());
		}
	}
	append_rule(tr);
}

Token* TagRuleLayer::get_next_token()
{
	Token* t = source()->get_next_token();
	if (t != NULL) {
		process(t);
	}
	return t;
}

void TagRuleLayer::process(Token* t)
{
	foreach (Lexeme& lex, t->lexemes()) {
		Tag newtag = lex.tag();
		foreach (const TagRule& tr, rules_) {
			tr.apply(newtag);
		}
		lex.set_tag(newtag);
	}
}


RegexTagRuleLayer::RegexTagRuleLayer(const Tagset& tagset)
	: TagRuleLayer(tagset), matcher_(), pattern_()
{
}

RegexTagRuleLayer::RegexTagRuleLayer(const Config::Node& cfg)
	: TagRuleLayer(cfg), matcher_(), pattern_()
{
	append_rule(cfg);
	std::string regexp_string = cfg.get("regexp", "");
	UnicodeString u_regexp_string = UnicodeString::fromUTF8(regexp_string);
	UErrorCode status = U_ZERO_ERROR;
	pattern_.reset(RegexPattern::compile(u_regexp_string, 0, status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
	matcher_.reset(pattern_->matcher(status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
}

RegexTagRuleLayer* RegexTagRuleLayer::clone() const
{
	RegexTagRuleLayer* copy = new RegexTagRuleLayer(tagset());
	if (matcher_) {
		copy->pattern_ = pattern_;
		UErrorCode status = U_ZERO_ERROR;
		copy->matcher_.reset(copy->pattern_->matcher(status));
	}
	return copy;
}

RegexTagRuleLayer::~RegexTagRuleLayer()
{
}

Token* RegexTagRuleLayer::get_next_token()
{
	Token* t = source()->get_next_token();
	if (t != NULL) {
		if (matcher_) {
			UErrorCode e = U_ZERO_ERROR;
			matcher_->reset(t->orth());
			if (matcher_->matches(e)) {
				process(t);
			}
		} else {
			process(t);
		}
	}
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
