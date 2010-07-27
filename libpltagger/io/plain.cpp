#include <libpltagger/io/plain.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	PlainWriter::PlainWriter(std::ostream& os, const Tagset& tagset)
		: os_(os), tagset_(tagset)
	{
	}

	void PlainWriter::write_token(const Token &t)
	{
		os_ << t.orth_utf8() << "\n";
		foreach (const Lexeme& lex, t.lexemes()) {
			os_ << "\t" << lex.lemma_utf8() << "\t" << tagset_.tag_to_string(lex.tag()) << "\n";
		}
	}

	void PlainWriter::write_sentence(const std::vector<Token *> &t)
	{
		os_ << "[[[\n[[[\n";
		foreach (const Token* tok, t) {
			write_token(*tok);
		}
		os_ << "]]]\n]]]\n";
	}

	PlainReader::PlainReader(std::istream& is, const Tagset& tagset)
		: is_(is), tagset_(tagset)
	{
	}
} /* end ns PlTagger */
