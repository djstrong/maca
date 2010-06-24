#include <libpltagger/conv/tagrulelayer.h>
#include <libtoki/foreach.h>
#include <libpltagger/tagsetmanager.h>

namespace PlTagger { namespace Conversion {

	TagRuleLayer::TagRuleLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), rules_()
	{
	}

	TagRuleLayer::TagRuleLayer(const Toki::Config::Node& cfg)
		: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset"))), rules_()
	{
		append_rule(cfg);
	}

	void TagRuleLayer::append_rule(const TagRule &tr)
	{
		rules_.push_back(tr);
	}

	void TagRuleLayer::append_rule(const Toki::Config::Node& cfg)
	{
		TagRule tr(tagset_from());
		foreach (const Toki::Config::Node::value_type &v, cfg) {
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
			foreach (const TagRule& tr, rules_) {
				tr.apply(*t);
			}
		}
		return t;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
