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
		: BufferedSentenceReader(sa->tagset()), is_(is), sa_(sa)
	{
		sa_->set_input_source(is);
	}

	Sentence* PlainReader::actual_next_sentence()
	{
		return sa_->get_next_sentence();
	}


} /* end ns Maca */
