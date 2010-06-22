#ifndef LIBPLTAGGER_TOKEN_LAYER_H
#define LIBPLTAGGER_TOKEN_LAYER_H

#include <libpltagger/tokensource.h>
#include <boost/function.hpp>
#include <vector>

namespace PlTagger { namespace Conversion {

	class Layer : public TokenSource
	{
	public:
		Layer();

		void set_source(TokenSource* src);

		TokenSource* source();

	private:
		TokenSource* source_;
	};

	class TagsetConverter
	{
	public:
		TagsetConverter();

		~TagsetConverter();

		void add_layer(Layer* cl);

		void convert(TokenSource* src, boost::function<void (Token*)> sink);

		template<class T>
		void convert_container(const T& container, boost::function<void (Token*)> sink)
		{
			TokenSource& src = make_range_source(container);
			convert(&src, sink);
		}
	private:
		std::vector<Layer*> layers_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_TOKEN_LAYER_H
