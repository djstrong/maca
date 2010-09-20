#ifndef LIBMACA_IO_READER_H
#define LIBMACA_IO_READER_H

#include <libmaca/chunk.h>
#include <libmaca/tokensource.h>
#include <deque>
#include <loki/Factory.h>
#include <loki/Singleton.h>

namespace Maca {

	/**
	 * Base class for readers of Maca tokens. Tokens are always read from a
	 * source and always within a given tagset.
	 *
	 * Tokens can be read on a token-by-token basis, or in entire senteces, or
	 * in entire chunks (paragraphs). Behavior is not defined if there are
	 * mixed calls to differend forms of getting data.
	 */
	class TokenReader : public TokenSource
	{
	public:
		TokenReader(const Tagset& tagset);

		virtual ~TokenReader();

		virtual Token* get_next_token() = 0;

		virtual Sentence* get_next_sentence() = 0;

		virtual Chunk* get_next_chunk() = 0;

		const Tagset& tagset() {
			return tagset_;
		}

	private:
		const Tagset& tagset_;
	};

	/**
	 * Convenience class for readers that keep a buffer of chunks. Sentence
	 * and token accesors are based upon the chunk buffer.
	 *
	 * A dervied class only neds to override ensure_more with a function that
	 * fills the chunk buffer.
	 */
	class BufferedChunkReader : public TokenReader
	{
	public:
		BufferedChunkReader(const Tagset& tagset);

		~BufferedChunkReader();

		Token* get_next_token();

		Sentence* get_next_sentence();

		Chunk* get_next_chunk();

	protected:
		virtual void ensure_more() = 0;

		std::deque<Chunk*> chunk_buf_;
		std::deque<Sentence*> sentence_buf_;
		std::deque<Token*> token_buf_;
	};

	/**
	 * Convenience class for readers that internally read sentences. The token
	 * accesor is based on the sentence buffer, so is the chunk accesor.
	 *
	 * A dervied class only neds to override actual_next_sentence with a
	 * function that returns a new sentence.
	 *
	 * Note that the chunk accesor might well read the entire input and return
	 * one huge chunk with all the sentences.
	 */
	class BufferedSentenceReader : public TokenReader
	{
	public:
		BufferedSentenceReader(const Tagset& tagset);

		Token* get_next_token();

		Sentence* get_next_sentence();

		Chunk* get_next_chunk();

	protected:
		virtual Sentence* actual_next_sentence() = 0;

		bool chunkify_;

		Sentence* sentence_buf_;

		std::deque<Token*> token_buf_;
	};


} /* end ns Maca */

#endif // LIBMACA_IO_READER_H
