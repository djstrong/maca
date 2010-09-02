#ifndef LIBMACA_IO_READER_H
#define LIBMACA_IO_READER_H

#include <libmaca/chunk.h>
#include <libmaca/tokensource.h>
#include <deque>

namespace Maca {

	/**
	 * Base class for readers of Maca tokens. Tokens are always read from a
	 * source and always within a given tagset.
	 *
	 * Tokens can be read on a token-by-token basis, or in entire senteces, or
	 * in entire chunks (paragraphs). Behavior is not defined if there are mixed
	 * calls to differend forms of getting data.
	 */
	class TokenReader : public TokenSource
	{
	public:
		TokenReader(std::istream& is, const Tagset& tagset);

		virtual ~TokenReader();

		virtual Token* get_next_token() = 0;

		virtual Sentence* get_next_sentence() = 0;

		virtual Chunk* get_next_chunk() = 0;

		std::istream& is() {
			return is_;
		}

		const Tagset& tagset() {
			return tagset_;
		}
	private:
		std::istream& is_;

		const Tagset& tagset_;
	};

	/**
	 * Convenience class for readers that keep a buffer of chunks
	 */
	class BufferedTokenReader : public TokenReader
	{
	public:
		BufferedTokenReader(std::istream& is, const Tagset& tagset);

		~BufferedTokenReader();

		Token* get_next_token();

		Sentence* get_next_sentence();

		Chunk* get_next_chunk();

	protected:
		virtual void ensure_more() = 0;

		std::deque<Chunk*> chunk_buf_;
		std::deque<Sentence*> sentence_buf_;
		std::deque<Token*> token_buf_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_READER_H
