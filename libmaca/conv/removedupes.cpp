#include <libmaca/conv/removedupes.h>
#include <libcorpus2/tagsetmanager.h>
#include <libcorpus2/token.h>

namespace Maca {
namespace Conversion {

RemoveDupesLayer::RemoveDupesLayer(const Corpus2::Tagset& tagset)
        : OneTagsetLayer(tagset)
{
}

RemoveDupesLayer::RemoveDupesLayer(const Config::Node& cfg)
        : OneTagsetLayer(Corpus2::get_named_tagset(cfg.get<std::string>("tagset")))
{
}

RemoveDupesLayer::~RemoveDupesLayer()
{
}

RemoveDupesLayer* RemoveDupesLayer::clone() const
{
        RemoveDupesLayer* copy = new RemoveDupesLayer(tagset());
        return copy;
}

Corpus2::Token* RemoveDupesLayer::get_next_token()
{
        Corpus2::Token* t = source()->get_next_token();
	if (t) {
		t->remove_duplicate_lexemes();
	}
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
