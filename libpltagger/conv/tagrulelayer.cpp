#include <libpltagger/conv/tagrulelayer.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TagRuleLayer::TagRuleLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset), rules_()
	{
	}

	void TagRuleLayer::append_rule(const TagRule &tr)
	{
		rules_.push_back(tr);
	}

	Token* TagRuleLayer::get_next_token()
	{
		Token* t = source()->get_next_token();
		if (t != NULL) {
			foreach (const TagRule& tr, rules_) {
				foreach (Lexeme& lex, t->lexemes()) {
					tr.apply(lex.tag());
				}
			}
		}
		return t;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
