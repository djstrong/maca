#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libcorpus2/io/reader.h>
#include <libmaca/util/sentenceanalyser.h>

namespace Maca {

	class PlainReader : public BufferedSentenceReader
	{
	public:
		PlainReader(std::istream& is,
				const boost::shared_ptr<SentenceAnalyser>& sa);

		std::istream& is() {
			return is_;
		}

	protected:
		/// BufferedSentenceReader override
		Sentence* actual_next_sentence();

		std::istream& is_;

		boost::shared_ptr<SentenceAnalyser> sa_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_PLAIN_H
