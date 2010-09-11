#ifndef LIBMACA_IO_PLAIN_H
#define LIBMACA_IO_PLAIN_H

#include <libmaca/io/reader.h>
#include <libmaca/io/writer.h>
#include <libmaca/util/sentenceanalyser.h>

namespace Maca {

	class PlainWriter : public TokenWriter
	{
	public:
		PlainWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params);

		void write_token(const Token& t);

		void write_sentence(const Sentence& t);

		void write_chunk(const Chunk& c);
	};

	class PlainReader : public TokenReader
	{
	public:
		PlainReader(std::istream& is, const boost::shared_ptr<SentenceAnalyser>& sa);

		Token* get_next_token();

		Sentence* get_next_sentence();

		Chunk* get_next_chunk();

	protected:
		boost::shared_ptr<SentenceAnalyser> sa_;

		bool chunkify_;

		Sentence* sentence_buf_;

		std::deque<Token*> token_buf_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_PLAIN_H
