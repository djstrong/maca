#ifndef LIBMACA_CONV_ATTRIBUTECOPIER_H
#define LIBMACA_CONV_ATTRIBUTECOPIER_H

#include <libcorpus2/token.h>

namespace Maca {
namespace Conversion {

/**
 * Helper function to creat a list of attributes given a tagset and a
 * string of atttribute names, colon or space separated.
 */
Corpus2::mask_t make_attributes_mask(const Corpus2::Tagset& tagset,
		const std::string& str);

/**
 * Helper function to creat a list of attributes given a tagset and a
 * string of atttribute names, colon or space separated. The attributes are
 * appended to the passed vector.
 */
Corpus2::mask_t append_attributes_mask(Corpus2::mask_t& v,
		const Corpus2::Tagset& tagset, const std::string& str);

/**
 * Helper function to copy some attributes from one tag to another
 */
void copy_attributes(const Corpus2::Tag& from,
		const Corpus2::mask_t& amask, Corpus2::Tag& to);

/**
 * Helper function to copy some attributes from one tag to another, for all
 * tags in a token.
 */
void copy_attributes(const Corpus2::Token& from,
		const Corpus2::mask_t& amask, Corpus2::Token& to);

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_ATTRIBUTECOPIER_H
