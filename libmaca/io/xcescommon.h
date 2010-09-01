#ifndef LIBMACA_IO_XCESCOMMON_H
#define LIBMACA_IO_XCESCOMMON_H

#include <libmaca/token.h>

namespace Maca {

	/**
	 * Output the token's XCES XML representation, ath the requested indent
	 * level. Use -1 to disable indentation.
	 */
	void token_as_xces_xml(std::ostream& os, const Tagset& tagset
			, const Token& t, int indent, bool force_disamb = false, bool sort = false);

	/**
	 * Output a xml-encoded version of the given string into the given ostream.
	 * The default XML entity substitutions are made: less than, greater than,
	 * ampersand, single quote and double quote.
	 */
	void encode_xml_entities_into(std::ostream& buf, const std::string& input);

	/**
	 * In-place XML entity encoding.
	 * The default XML entity substitutions are made: less than, greater than,
	 * ampersand, single quote and double quote.
	 */
	void encode_xml_entities(std::string &input);

} /* end ns Maca */

#endif // LIBMACA_IO_XCESCOMMON_H
