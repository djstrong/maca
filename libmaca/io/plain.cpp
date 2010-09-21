#include <libmaca/io/plain.h>
#include <libpwrutils/foreach.h>

namespace Maca {

	PlainReader::PlainReader(std::istream &is,
			const boost::shared_ptr<SentenceAnalyser>& sa)
		: BufferedSentenceReader(sa->tagset()), is_(is), sa_(sa)
	{
		sa_->set_input_source(is);
	}

	Sentence* PlainReader::actual_next_sentence()
	{
		return sa_->get_next_sentence();
	}


} /* end ns Maca */
