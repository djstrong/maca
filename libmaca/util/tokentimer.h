#ifndef LIBMACA_UTIL_TOKENTIMER_H
#define LIBMACA_UTIL_TOKENTIMER_H

#include <ctime>
#include <libmaca/chunk.h>

namespace Maca {

	/**
	 * Utility class for token counting and timing, useful in progress info.
	 */
	class TokenTimer
	{
	public:
		/// Constructor
		TokenTimer();

		/// count one token
		void count_token() {
			++tokens_;
			++slice_tokens_;
		}

		/// count a sentence whose tokens are counted separetely
		void count_sentence() {
			++sentences_;
		}

		/// count a sentence with tokens
		void count_sentence(const Sentence& s) {
			++sentences_;
			tokens_ += s.size();
			slice_tokens_ += s.size();
		}

		/// count a chunk with sentences with tokens
		void count_chunk(const Chunk& c) {
			for (size_t i = 0; i < c.sentences().size(); ++i) {
				count_sentence(*c.sentences()[i]);
			}
		}

		/// reset the timer
		void reset();

		/// check if at least s seconds have elapsed since the last
		/// slice began and if yes, display progress info and start a new slice
		bool check_slice(int s = 1);

		/// display current stats
		void stats();

	private:
		size_t tokens_;
		size_t slice_tokens_;
		size_t sentences_;
		clock_t start_;
		clock_t slice_start_;
	};
} /* end ns Maca */

#endif // LIBMACA_UTIL_TOKENTIMER_H
