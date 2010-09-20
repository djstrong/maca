#include <libmaca/io/reader.h>

namespace Maca {

	TokenReader::TokenReader(const Tagset& tagset)
		: tagset_(tagset)
	{
	}

	TokenReader::~TokenReader()
	{
	}

	BufferedChunkReader::BufferedChunkReader(const Tagset& tagset)
		: TokenReader(tagset)
	{
	}

	BufferedChunkReader::~BufferedChunkReader()
	{
		foreach (Token* t, token_buf_) {
			delete t;
		}
		foreach (Sentence* s, sentence_buf_) {
			delete s;
		}
		foreach (Chunk* c, chunk_buf_) {
			delete c;
		}
	}

	Token* BufferedChunkReader::get_next_token()
	{
		bool more = true;
		while (token_buf_.empty() && more) {
			ensure_more();
			Sentence* s = get_next_sentence();
			if (s != NULL) {
				std::copy(s->tokens().begin(), s->tokens().end(),
						std::back_inserter(token_buf_));
			} else {
				more = false;
			}
		}
		if (token_buf_.empty()) {
			return NULL;
		} else {
			Token* t = token_buf_.front();
			token_buf_.pop_front();
			return t;
		}
	}

	Sentence* BufferedChunkReader::get_next_sentence()
	{
		bool more = true;
		while (sentence_buf_.empty() && more) {
			ensure_more();
			Chunk* c = get_next_chunk();
			if (c != NULL) {
				std::copy(c->sentences().begin(), c->sentences().end(),
						std::back_inserter(sentence_buf_));
			} else {
				more = false;
			}
		}
		if (sentence_buf_.empty()) {
			return NULL;
		} else {
			Sentence* s = sentence_buf_.front();
			sentence_buf_.pop_front();
			return s;
		}
	}

	Chunk* BufferedChunkReader::get_next_chunk()
	{
		ensure_more();
		if (chunk_buf_.empty()) {
			return NULL;
		} else {
			Chunk* t = chunk_buf_.front();
			chunk_buf_.pop_front();
			return t;
		}
	}

	BufferedSentenceReader::BufferedSentenceReader(const Tagset& tagset)
		: TokenReader(tagset), chunkify_(true)
		, sentence_buf_(NULL), token_buf_()
	{
	}

	Token* BufferedSentenceReader::get_next_token()
	{
		bool more = true;
		while (token_buf_.empty() && more) {
			Sentence* s = get_next_sentence();
			if (s != NULL) {
				std::copy(s->tokens().begin(), s->tokens().end(),
					std::back_inserter(token_buf_));
			} else {
				more = false;
			}
		}
		if (token_buf_.empty()) {
			return NULL;
		} else {
			Token* t = token_buf_.front();
			token_buf_.pop_front();
			return t;
		}
	}

	Sentence* BufferedSentenceReader::get_next_sentence()
	{
		if (sentence_buf_ != NULL) {
			Sentence* s = sentence_buf_;
			sentence_buf_ = NULL;
			return s;
		} else {
			return actual_next_sentence();
		}
	}

	Chunk* BufferedSentenceReader::get_next_chunk()
	{
		Sentence* s = get_next_sentence();
		if (s == NULL) {
			return NULL;
		} else {
			Chunk* c = new Chunk;
			c->append(s);
			s = get_next_sentence();
			while (s != NULL && (!chunkify_ || s->first_token()->wa() !=
					Toki::Whitespace::ManyNewlines)) {
				c->append(s);
				s = get_next_sentence();
			}
			if (s != NULL) {
				sentence_buf_ = s;
			}
			return c;
		}
	}



} /* end ns Maca */
