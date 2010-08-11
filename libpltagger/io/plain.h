#ifndef LIBPLTAGGER_IO_PLAIN_H
#define LIBPLTAGGER_IO_PLAIN_H

#include <libpltagger/io/reader.h>
#include <libpltagger/io/writer.h>

namespace PlTagger {

	class PlainWriter : public TokenWriter
	{
	public:
		PlainWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params);

		void write_token(const Token& t);

		void write_sentence(const Sentence& t);

		void write_chunk(const Chunk& c);
	};

	class PlainReader
	{
	public:
		PlainReader(std::istream& is, const Tagset& tagset);

		Token* read_token();

	private:
		std::istream& is_;

		const Tagset& tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_PLAIN_H
