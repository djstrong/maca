#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libmaca/io/reader.h>
#include <libmaca/io/writer.h>
#include <libmaca/util/sentenceanalyser.h>

namespace Maca {

	class PlainWriter : public TokenWriter
	{
	public:
		PlainWriter(std::ostream& os, const Tagset& tagset,
				const string_range_vector& params);

		void write_token(const Token& t);

		void write_sentence(const Sentence& t);

		void write_chunk(const Chunk& c);
	};

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
