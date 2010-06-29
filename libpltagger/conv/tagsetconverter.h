#ifndef LIBPLTAGGER_TAGSETCONVERTER_H
#define LIBPLTAGGER_TAGSETCONVERTER_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <libpltagger/tokensource.h>
#include <libpltagger/confignode.h>
#include <boost/function.hpp>

namespace PlTagger { namespace Conversion {

	class Layer;

	class TagsetConverter : private boost::noncopyable
	{
	public:
		TagsetConverter();

		TagsetConverter(const Config::Node& cfg);

		virtual ~TagsetConverter();

		size_t layer_count() const {
			return layers_.size();
		}

		void add_layer(Layer* cl);

		const Tagset& tagset_from() const;

		const Tagset& tagset_to() const;

		void convert(TokenSource* src, boost::function<void (Token*)> sink);

		template<class T>
		void convert_container(const T& container, boost::function<void (Token*)> sink)
		{
			RangeSource< boost::iterator_range<typename T::const_iterator> > src(container);
			convert(&src, sink);
		}

		virtual void convert_simple(const std::vector<Token*>& v,
				boost::function<void (Token*)> sink);

		virtual void convert_ambiguous(const std::vector< std::vector<Token*> >& v,
				boost::function<void (Token*)> sink);

	private:
		std::vector<Layer*> layers_;
	};
} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSETCONVERTER_H
