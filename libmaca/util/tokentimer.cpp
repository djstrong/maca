#include <libmaca/util/tokentimer.h>
#include <iostream>

namespace Maca {

	TokenTimer::TokenTimer()
		: tokens_(0), slice_tokens_(0), sentences_(0)
		, start_(clock()), slice_start_(start_)
	{
	}

	void TokenTimer::reset()
	{
		tokens_ = slice_tokens_ = sentences_ = 0;
		start_ = slice_start_ = clock();
	}

	bool TokenTimer::check_slice()
	{
		clock_t now_clock = clock();
		if (now_clock - CLOCKS_PER_SEC > slice_start_) {
			double slice_elapsed = ((double)now_clock - slice_start_) / CLOCKS_PER_SEC;
			double elapsed = ((double)now_clock - start_) / CLOCKS_PER_SEC;
			double slice_rate = slice_tokens_ / slice_elapsed;
			double avg_rate = tokens_ / elapsed;
			slice_tokens_ = 0;
			slice_start_ = now_clock;
			std::cerr << "\r" << "Processed " << sentences_ << " sentences, "
					<< tokens_ << " tokens, "
					<< "rate " << slice_rate << " t/s, avg " << avg_rate << " t/s    ";
			return true;
		} else {
			return false;
		}
	}

	void TokenTimer::stats()
	{
		clock_t now_clock = clock();
		double elapsed = ((double)now_clock - start_) / CLOCKS_PER_SEC;
		double avg_rate = tokens_ / elapsed;
		std::cerr << "\r" << "Processed " << sentences_ << " sentences, "
				<< tokens_ << " tokens, "
				<< "avg rate " << avg_rate << " t/s                    \n";
	}

} /* end ns Maca */
