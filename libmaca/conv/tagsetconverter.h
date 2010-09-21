#ifndef LIBMACA_TAGSETCONVERTER_H
#define LIBMACA_TAGSETCONVERTER_H

#include <libcorpus2/sentence.h>
#include <libcorpus2/tagset.h>
#include <libmaca/tokensource.h>
#include <libmaca/util/confignode.h>
#include <boost/function.hpp>

namespace Maca {
namespace Conversion {

class Layer;

/**
 * The master tagset converter class which accepts tokens with tags in one
 * tagset and outputs tokens in another tagset. The processing is layer
 * based, with each layer capable of modyfying tagis of a token and also
 * possibly joining tokens together or splitting them.
 */
class TagsetConverter : private boost::noncopyable
{
public:
	/**
	 * Empty converter constructor
	 */
	TagsetConverter();

	/**
	 * Config node constructor
	 */
	TagsetConverter(const Config::Node& cfg);

	/// Destructor
	virtual ~TagsetConverter();

	TagsetConverter* clone() const;

	/// Layers count accesor
	size_t layer_count() const {
		return layers_.size();
	}

	/**
	 * Layer adder. The output tagset of the last layer must match the
	 * input layer of the layer being added, the first layer's input tagset
	 * is the input tagset of the entire converter, and similarily the
	 * output tagset of the las layer is the output tagset of the
	 * converter.
	 */
	void add_layer(Layer* cl);

	/**
	 * Input tagset, equivalent to the input tagset of the first layer.
	 */
	const Tagset& tagset_from() const;

	/**
	 * Output tagset, equivalent to the output tagset of the last layer.
	 */
	const Tagset& tagset_to() const;

	/**
	 * The main conversion function, output tokens are fed into the sink.
	 */
	void convert(TokenSource* src, boost::function<void (Token*)> sink);

	/**
	 * Helper to convert a container.
	 */
	template<class T>
	void convert_container(const T& container,
			boost::function<void (Token*)> sink)
	{
		RangeSource<
			boost::iterator_range<typename T::const_iterator>
		> src(container);
		convert(&src, sink);
	}

	/**
	 * Helper to convert a single path
	 */
	void convert_simple(const std::vector<Token*>& v,
			boost::function<void (Token*)> sink);

	/**
	 * Helper to convert a multi-path analysis, with path folding if
	 * possible and shortest-path selection as a fallback.
	 */
	void convert_ambiguous(const std::vector< std::vector<Token*> >& v,
			boost::function<void (Token*)> sink,
			bool warn_on_failure = true);

	Sentence* convert_sentence(Sentence* s);

private:
	/// The conversion layers
	std::vector<Layer*> layers_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_TAGSETCONVERTER_H
