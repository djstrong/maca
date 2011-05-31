#ifndef LIBMACA_CONV_REMOVEDUPES_H
#define LIBMACA_CONV_REMOVEDUPES_H

#include <libmaca/conv/layer.h>
#include <libmaca/util/confignode.h>

namespace Maca {
namespace Conversion {

/**
 * A layer that removes any lexeme duplicates in tokens
 */
class RemoveDupesLayer : public OneTagsetLayer
{
public:
        /**
         * Constructor for a RemoveDupesLayer working within a tagset.
         */
        RemoveDupesLayer(const Corpus2::Tagset& tagset);

        /**
         * Config node constructor. The config node is passed to the parent
         * class, and then a RemoveDupesLayer is created from it.
         */
        RemoveDupesLayer(const Config::Node& cfg);

        /// Destructor
        ~RemoveDupesLayer();

        /// Cloning
        RemoveDupesLayer* clone() const;

        /// Layer override
        Corpus2::Token* get_next_token();

private:
};


} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_REMOVEDUPES_H
