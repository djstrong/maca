#include <libpltagger/io/plain.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	PlainWriter::PlainWriter(std::ostream& os, const Tagset& tagset)
		: TokenWriter(os, tagset)
	{
	}

	void PlainWriter::do_token(const Token &t)
	{
		os() << t.orth_utf8() << "\n";
		foreach (const Lexeme& lex, t.lexemes()) {
			os() << "\t" << lex.lemma_utf8() << "\t" << tagset().tag_to_string(lex.tag()) << "\n";
		}
	}
	void PlainWriter::do_sentence(const std::vector<Token *> &t)
	{
		os() << "[[[\n";
		TokenWriter::do_sentence(t);
		os() << "]]]\n";
	}

	void PlainWriter::do_paragraph(const std::vector< std::vector<Token *> > &t)
	{
		os() << "[[[<<<\n\n";
		TokenWriter::do_paragraph(t);
		os() << ">>>]]]\n\n";
	}

	PlainReader::PlainReader(std::istream& is, const Tagset& tagset)
		: is_(is), tagset_(tagset)
	{
	}
} /* end ns PlTagger */
