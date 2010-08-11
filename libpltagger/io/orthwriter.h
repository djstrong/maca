#ifndef LIBPLTAGGER_IO_ORTHWRITER_H
#define LIBPLTAGGER_IO_ORTHWRITER_H

#include <libpltagger/io/writer.h>

namespace PlTagger {

	class OrthWriter : public TokenWriter {
	public:
		OrthWriter(std::ostream& os, const Tagset& tagset);

		~OrthWriter();

		void write_token(const Token &t);

		void write_sentence(const Sentence &s);

		void write_chunk(const Chunk &c);
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_ORTHWRITER_H
