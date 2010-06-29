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
	 * for possible joining with thenext token that will be processed from the
	 * source.
	 */
	class JoinLayer : public OneTagsetLayer
	{
	public:
		JoinLayer(const Tagset& tagset);

		JoinLayer(const Config::Node& cfg);

		void append_rule(const JoinRule& rule);

		void append_rule(const Config::Node& cfg);

		Token* get_next_token();

	private:
		std::vector<JoinRule> rules_;

		Token* buf_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINLAYER_H
