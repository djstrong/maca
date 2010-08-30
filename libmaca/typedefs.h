#ifndef LIBMACA_TYPEDEFS_H
#define LIBMACA_TYPEDEFS_H

#include <boost/range.hpp>

#include <string>
#include <vector>

namespace Maca {

	typedef boost::iterator_range<std::string::const_iterator> string_range;

	typedef std::vector<string_range> string_range_vector;

	typedef boost::iterator_range<std::string::iterator> string_range_mutable;

} /* end ns Maca */

#endif // LIBMACA_TYPEDEFS_H
