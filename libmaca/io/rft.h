#ifndef LIBMACA_IO_RFT_H
#define LIBMACA_IO_RFT_H

#include <libmaca/io/reader.h>
#include <libmaca/io/writer.h>
#include <libmaca/util/sentenceanalyser.h>

namespace Maca {

	/**
	 * Simple writer class to output token in RFTagger-compatible corpora form.
	 *
	 * One token per line, token line consists of the orth, followed by the
	 * tag, followed by newline (one tag per token only). Each sentence
	 * is followed by a blank line.
	 *
	 * The first lexeme is used. No-lexeme tokens trigger a warning unless
	 * nowarn is passed.
	 */
	class RftWriter : public TokenWriter
	{
	public:
		RftWriter(std::ostream& os, const Tagset& tagset,
				const string_range_vector& params);

		void write_token(const Token& t);

		void write_sentence(const Sentence& s);

		void write_chunk(const Chunk &p);

		static bool registered;

	private:
		bool warn_on_no_lexemes_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_RFT_H
