#ifndef LIBPLTAGGER_CONV_ATTRIBUTECOPIER_H
#define LIBPLTAGGER_CONV_ATTRIBUTECOPIER_H

#include <libpltagger/token.h>

namespace PlTagger { namespace Conversion {

	std::vector<attribute_idx_t> make_attribute_list(const Tagset& tagset, const std::string& str);

	void copy_attributes(const Tag& from, const std::vector<attribute_idx_t>& alist, Tag& to);

	void copy_attributes(const Token& from, const std::vector<attribute_idx_t>& alist, Token& to);

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_ATTRIBUTECOPIER_H
