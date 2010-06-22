#ifndef LIBPLTAGGER_CONV_FOLD_H
#define LIBPLTAGGER_CONV_FOLD_H

#include <vector>

namespace PlTagger {

	class Token;

	namespace Conversion {

		size_t find_shortest(const std::vector<std::vector<Token *> >& v, size_t& min_len_path);

		bool try_fold_paths(const std::vector< std::vector<Token*> >& v);

		std::vector<Token*> choose_path(const std::vector< std::vector<Token*> >& v, size_t n);

		std::vector<Token*> choose_shortest_path(const std::vector< std::vector<Token*> >& v);

	} /* end ns Conversion */

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_FOLD_H
