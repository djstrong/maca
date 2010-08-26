#ifndef LIBPLTAGGER_TOKEN_SOURCE_H
#define LIBPLTAGGER_TOKEN_SOURCE_H

#include <libtoki/tokensource.h>

namespace PlTagger {

	/// forward declaration
	class Token;

	/// typedef for a PlTagger Token TokenSource, based on the toki template
	typedef Toki::TokenSourceTemplate<Token> TokenSource;

	/// pull RangeSource and make_range_source into PlTagger namespace
	using Toki::RangeSource;
	using Toki::make_range_source;

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TOKEN_SOURCE_H
