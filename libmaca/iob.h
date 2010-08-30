#ifndef LIBMACA_IOB_H
#define LIBMACA_IOB_H

#include <string>
#include <vector>

namespace Maca {

	namespace Iob {
		static const int I = -1;
		static const int O = 0;
		static const int B = 1;

		inline bool is_i(int iob) {
			return iob == I;
		}

		inline bool is_o(int iob) {
			return iob == O;
		}

		inline bool is_b(int iob) {
			return iob > 0;
		}

		std::string to_iob_string(int iob);
	}

	class IobChannel {
	public:
		IobChannel();
	private:
		std::vector<int> data_;
	};

} /* end ns Maca */

#endif // LIBMACA_IOB_H
