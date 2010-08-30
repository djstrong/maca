#ifndef LIBMACA_IO_ORTHWRITER_H
#define LIBMACA_IO_ORTHWRITER_H

#include <libmaca/io/writer.h>

namespace Maca {

	class OrthWriter : public TokenWriter {
	public:
		OrthWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params);

		~OrthWriter();

		void write_token(const Token &t);

		void write_sentence(const Sentence &s);

		void write_chunk(const Chunk &c);
	};

} /* end ns Maca */

#endif // LIBMACA_IO_ORTHWRITER_H
