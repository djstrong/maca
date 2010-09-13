#include <libmaca/io/plain.h>
#include <libtoki/util/foreach.h>

namespace Maca {

	PlainWriter::PlainWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params)
		: TokenWriter(os, tagset, params)
	{
	}

	void PlainWriter::write_token(const Token &t)
	{
		os() << t.orth_utf8() << "\n";
		foreach (const Lexeme& lex, t.lexemes()) {
			os() << "\t" << lex.lemma_utf8() << "\t"
				<< tagset().tag_to_string(lex.tag()) << "\n";
		}
	}
	void PlainWriter::write_sentence(const Sentence &s)
	{
		os() << "[[[\n";
		foreach (const Token* t, s.tokens()) {
			write_token(*t);
		}
		os() << "]]]\n";
	}

	void PlainWriter::write_chunk(const Chunk& c)
	{
		os() << "[[[<<<\n\n";
		foreach (const Sentence* s, c.sentences()) {
			write_sentence(*s);
		}
		os() << ">>>]]]\n\n";
	}

	PlainReader::PlainReader(std::istream &is,
			const boost::shared_ptr<SentenceAnalyser>& sa)
		: TokenReader(is, sa->tagset()), sa_(sa), chunkify_(true)
		, sentence_buf_(NULL), token_buf_()
	{
		sa_->set_input_source(is);
	}

	Token* PlainReader::get_next_token()
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

	Sentence* PlainReader::get_next_sentence()
	{
		if (sentence_buf_ != NULL) {
			Sentence* s = sentence_buf_;
			sentence_buf_ = NULL;
			return s;
		} else {
			return sa_->get_next_sentence();
		}
	}

	Chunk* PlainReader::get_next_chunk()
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
