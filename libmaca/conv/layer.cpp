#include <libmaca/conv/layer.h>

namespace Maca {
namespace Conversion {

Layer::Layer()
	: source_(NULL)
{
}

Layer::~Layer()
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

OneTagsetLayer::OneTagsetLayer(const Tagset& tagset)
	: tagset_(tagset)
{
}

const Tagset& OneTagsetLayer::tagset_from() const
{
	return tagset_;
}

const Tagset& OneTagsetLayer::tagset_to() const
{
	return tagset_;
}

} /* end ns Conversion */
} /* end ns Maca */
