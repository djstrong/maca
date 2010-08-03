#include <libpltagger/iob.h>

namespace PlTagger {
	namespace Iob {
		std::string to_iob_string(int iob)
		{
			if (is_i(iob)) {
				return "I";
			} else if (is_o(iob)) {
				return "O";
			} else {
				return "B";
			}
		}

	}
} /* end ns PlTagger */