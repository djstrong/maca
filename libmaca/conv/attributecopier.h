#ifndef LIBMACA_CONV_ATTRIBUTECOPIER_H
#define LIBMACA_CONV_ATTRIBUTECOPIER_H

#include <libmaca/token.h>

namespace Maca { namespace Conversion {

	/**
	 * Helper function to creat a list of attributes given a tagset and a
	 * string of atttribute names, colon or space separated.
	 */
	std::vector<attribute_idx_t> make_attribute_list(const Tagset& tagset, const std::string& str);

	/**
	 * Helper function to creat a list of attributes given a tagset and a
	 * string of atttribute names, colon or space separated. The attributes are
	 * appended to the passed vector.
	 */
	void append_attribute_list(std::vector<attribute_idx_t>& v, const Tagset& tagset, const std::string& str);

	/**
	 * Helper function to copy some attributes from one tag to another
	 */
	void copy_attributes(const Tag& from, const std::vector<attribute_idx_t>& alist, Tag& to);

	/**
	 * Helper function to copy some attributes from one tag to another, for all
	 * tags in a token.
	 */
	void copy_attributes(const Token& from, const std::vector<attribute_idx_t>& alist, Token& to);

} /* end ns Conversion */ } /* end ns Maca */

#endif // LIBMACA_CONV_ATTRIBUTECOPIER_H
