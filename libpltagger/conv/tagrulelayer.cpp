#include <libpltagger/conv/tagrulelayer.h>
#include <libtoki/util/foreach.h>
#include <libpltagger/tagsetmanager.h>

namespace PlTagger { namespace Conversion {

	TagRuleLayer::TagRuleLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), rules_()
	{
	}

	TagRuleLayer::TagRuleLayer(const Config::Node& cfg)
		: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset"))), rules_()
	{
		append_rule(cfg);
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
		foreach (const TagRule& tr, rules_) {
			tr.apply(*t);
		}
	}


	RegexTagRuleLayer::RegexTagRuleLayer(const Tagset& tagset)
		: TagRuleLayer(tagset), matcher_(NULL)
	{
	}

	RegexTagRuleLayer::RegexTagRuleLayer(const Config::Node& cfg)
		: TagRuleLayer(cfg), matcher_(NULL)
	{
		append_rule(cfg);
		std::string regexp_string = cfg.get("regexp", "");
		UErrorCode status = U_ZERO_ERROR;
		matcher_ = new RegexMatcher(UnicodeString::fromUTF8(regexp_string), 0, status);
		if (!U_SUCCESS(status)) throw PlTaggerError("Regexp failed to compile: " + regexp_string);
	}

	RegexTagRuleLayer::~RegexTagRuleLayer()
	{
		delete matcher_;
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

} /* end ns Conversion */ } /* end ns PlTagger */
