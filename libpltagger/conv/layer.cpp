#include <libpltagger/conv/layer.h>

namespace PlTagger { namespace Conversion {

	Layer::Layer()
		: source_(NULL)
	{
	}

	void Layer::set_source(TokenSource *src)
	{
		source_ = src;
	}

	TokenSource* Layer::source()
	{
		return source_;
	}

	void TagsetConverter::convert(TokenSource* src, boost::function<void (Token*)> sink)
	{
		assert(!layers_.empty());
		assert(layers_.back()->get_next_token() == NULL);
		layers_.front()->set_source(src);
		Token* t;
		while ((t = layers_.back()->get_next_token())) {
			sink(t);
		}
	}

} /* end ns Conversion */ } /* end ns PlTagger */
