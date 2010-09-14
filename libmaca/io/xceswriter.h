#ifndef LIBMACA_IO_XCESWRITER_H
#define LIBMACA_IO_XCESWRITER_H

#include <libmaca/io/writer.h>

namespace Maca {

	class XcesWriter : public TokenWriter {
	public:
		XcesWriter(std::ostream& os, const Tagset& tagset,
				const string_range_vector& params);

		~XcesWriter();

		void write_token(const Token &t);

		void write_sentence(const Sentence &s);

		void write_chunk(const Chunk &c);

	protected:
		void do_header();

		void do_footer();

		void paragraph_head();

		void paragraph_head(const Chunk& c);

		int cid_;

		bool use_indent_;

		bool force_chunk_;

		bool force_disamb_;

		bool sort_tags_;

		bool split_chunks_on_newlines_;

		bool whitespace_info_;
	};


} /* end ns Maca */

#endif // LIBMACA_IO_XCESWRITER_H
