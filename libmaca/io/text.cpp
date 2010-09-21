#include <libmaca/io/text.h>
#include <libpwrutils/foreach.h>

namespace Maca {

TextReader::TextReader(std::istream &is,
		const boost::shared_ptr<SentenceAnalyser>& sa)
	: Corpus2::BufferedSentenceReader(sa->tagset()), is_(is), sa_(sa)
{
	sa_->set_input_source(is);
}

Corpus2::Sentence* TextReader::actual_next_sentence()
{
	return sa_->get_next_sentence();
}


} /* end ns Maca */
