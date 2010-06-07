#ifndef LIBPLTAGGER_TYPEDEFS_H
#define LIBPLTAGGER_TYPEDEFS_H

#include <boost/range.hpp>

#include <string>
#include <vector>

namespace PlTagger {

	typedef boost::iterator_range<std::string::const_iterator> string_range;

	typedef std::vector<string_range> string_range_vector;

	typedef boost::iterator_range<std::string::iterator> string_range_mutable;

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TYPEDEFS_H
