#ifndef LIBPLTAGGER_CONV_JOINLAYER_H
#define LIBPLTAGGER_CONV_JOINLAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/joinrule.h>

namespace PlTagger { namespace Conversion {

	class JoinLayer : public Layer
	{
	public:
		JoinLayer();

		void append_rule(const JoinRule& rule);

		Token* get_next_token();

	private:
		std::vector<JoinRule> rules_;

		Token* next_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_JOINLAYER_H
