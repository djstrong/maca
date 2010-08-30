#ifndef LIBMACA_TOKEN_SOURCE_H
#define LIBMACA_TOKEN_SOURCE_H

#include <libtoki/tokensource.h>

namespace Maca {

	/// forward declaration
	class Token;

	/// typedef for a Maca Token TokenSource, based on the toki template
	typedef Toki::TokenSourceTemplate<Token> TokenSource;

	/// pull RangeSource and make_range_source into Maca namespace
	using Toki::RangeSource;
	using Toki::make_range_source;

} /* end ns Maca */

#endif // LIBMACA_TOKEN_SOURCE_H
