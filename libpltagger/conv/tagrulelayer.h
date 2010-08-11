#ifndef LIBPLTAGGER_CONV_TAGRULELAYER_H
#define LIBPLTAGGER_CONV_TAGRULELAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/tagrule.h>
#include <libpltagger/util/confignode.h>

namespace PlTagger { namespace Conversion {

	/**
	 * A layer of TagRules which are applied in order
	 */
	class TagRuleLayer : public OneTagsetLayer
	{
	public:
		/**
		 * Empty layer constructor
		 */
		TagRuleLayer(const Tagset& tagset);

		/**
		 * Constructor from a config node. Creates a layer and uses the config
		 * to create one tag rule.
		 */
		TagRuleLayer(const Config::Node& cfg);

		/**
		 * Rule adder
		 */
		void append_rule(const TagRule& tr);

		/**
		 * Rule adder, the rule is constructed from the config node
		 * Recognized keys are:
		 * - pre - a precondition predicate string, can appear multiple times
		 * - post - a postcondition predicate string, can appear multiple times
		 */
		void append_rule(const Config::Node& cfg);

		/**
		 * Layer override -- gar tokens from source, pass them through the
		 * tag rules and return.
		 */
		Token* get_next_token();

	private:
		/// The TagRules used by this layer
		std::vector<TagRule> rules_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV/TAGRULELAYER_H
