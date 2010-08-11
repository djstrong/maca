#ifndef LIBPLTAGGER_IO_XCESCOMMON_H
#define LIBPLTAGGER_IO_XCESCOMMON_H

#include <libpltagger/token.h>

namespace PlTagger {

	/**
	 * Output the token's XCES XML representation, ath the requested indent
	 * level. Use -1 to disable indentation.
	 */
	void token_as_xces_xml(std::ostream& os, const Tagset& tagset
			, const Token& t, int indent, bool force_disamb = false, bool sort = false);

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCESCOMMON_H
