#include <libmaca/io/reader.h>

namespace Maca {

	TokenReader::TokenReader(std::istream& is, const Tagset& tagset)
		: is_(is), tagset_(tagset)
	{
	}

	TokenReader::~TokenReader()
	{
	}

	BufferedTokenReader::BufferedTokenReader(std::istream& is,
			const Tagset& tagset)
		: TokenReader(is, tagset)
	{
	}

	BufferedTokenReader::~BufferedTokenReader()
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

	Token* BufferedTokenReader::get_next_token()
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

	Sentence* BufferedTokenReader::get_next_sentence()
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

	Chunk* BufferedTokenReader::get_next_chunk()
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

} /* end ns Maca */
