#ifndef LIBPLTAGGER_CONV_JOINLAYER_H
#define LIBPLTAGGER_CONV_JOINLAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/joinrule.h>
#include <libpltagger/confignode.h>

namespace PlTagger { namespace Conversion {

	/**
	 * A layer of JoinRules, each of which is tried on every pair of consecutive
	 * tokens that appear from the source of the layer. If no rule joins the
	 * tokens, the first one is output unchanged and the second one retained
	 * for possible joining with the next token that will be processed from the
	 * source. The rules are checked in order, the first matching one is used.
	 *
	 * @see JoinRule for details of how tokens are joined.
	 */
	class JoinLayer : public OneTagsetLayer
	{
	public:
		/**
		 * Constructor for an empty JoinLayer working within a tagset.
		 *
		 * An empty JoinLayer has no rules so it simply will never join tokens.
		 */
		JoinLayer(const Tagset& tagset);

		/**
		 * Config node constructor. The config node is passed to the parent
		 * class, and then one JoinRule is created from it.
		 */
		JoinLayer(const Config::Node& cfg);

		/**
		 * Adds a rule to the list of join rules in this layer.
		 *
		 * The tagset of the JoinRule must match this layer's tagset.
		 */
		void append_rule(const JoinRule& rule);

		/**
		 * Creates a JoinRule from a config node and adds it to the list of
		 * join rules. Tagsets must match.
		 */
		void append_rule(const Config::Node& cfg);

		/// Layer override
		Token* get_next_token();

	private:
		/// The rules
		std::vector<JoinRule> rules_;

		/// buffer for the token that might be joined with the next one
		Token* buf_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINLAYER_H
