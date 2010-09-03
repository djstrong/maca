#ifndef LIBMACA_CONV_TAGRULELAYER_H
#define LIBMACA_CONV_TAGRULELAYER_H

#include <libmaca/conv/layer.h>
#include <libmaca/conv/tagrule.h>
#include <libmaca/util/confignode.h>
#include <unicode/regex.h>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace Maca { namespace Conversion {

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

		/// Cloning
		TagRuleLayer* clone() const;

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
		 * Layer override -- get tokens from source, pass them through the
		 * tag rules and return.
		 */
		Token* get_next_token();

	protected:
		/// Pass a token through the rules
		void process(Token*);

	private:
		/// The TagRules used by this layer
		std::vector<TagRule> rules_;
	};

	class RegexTagRuleLayer : public TagRuleLayer
	{
	public:
		/**
		 * Empty layer constructor
		 */
		RegexTagRuleLayer(const Tagset& tagset);

		/**
		 * Constructor from a config node. Creates a layer and uses the config
		 * to create one tag rule with a regex.
		 */
		RegexTagRuleLayer(const Config::Node& cfg);

		/// Cloning
		RegexTagRuleLayer* clone() const;

		/// Destructor
		~RegexTagRuleLayer();

		/**
		 * Layer override -- get tokens from source, pass them through the
		 * tag rules and return.
		 */
		Token* get_next_token();

	private:
		boost::scoped_ptr<RegexMatcher> matcher_;

		boost::shared_ptr<RegexPattern> pattern_;
	};

} /* end ns Conversion */ } /* end ns Maca */

#endif // LIBMACA_CONV/TAGRULELAYER_H
