#include <libpltagger/io/xces.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	XcesWriter::XcesWriter(std::ostream& os, const Tagset& tagset)
		: os_(os), tagset_(tagset), needs_footer_(true), indent_(0), cid_(0)
	{
		header();
	}

	XcesWriter::~XcesWriter()
	{
		finish();
	}

	void XcesWriter::finish()
	{
		if (needs_footer_) {
			footer();
			needs_footer_ = false;
		}
	}

	void XcesWriter::reset()
	{
		header();
		needs_footer_ = true;
	}

	void XcesWriter::write_sentence(const std::vector<Token *>& v)
	{
		paragraph_head();
		++indent_;
		sentence(v);
		--indent_;
		iwrite("</chunk>\n");
	}

	void XcesWriter::write_paragraph(const std::vector<std::vector<Token *> > &v)
	{
		paragraph(v);
	}

	void XcesWriter::iwrite(const std::string &s)
	{
		indent();
		os_ << s;
	}

	void XcesWriter::indent()
	{
		for (int i = 0; i < indent_; ++i) {
			os_ << " ";
		}
	}

	void XcesWriter::header()
	{
		os_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		os_ << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
		os_ << "<cesAna version=\"1.0\" type=\"lex disamb\">\n";
		os_ << "<chunkList>\n";
	}

	void XcesWriter::footer()
	{
		os_ << "</chunkList>\n";
		os_ << "</cesAna>\n";
	}

	void XcesWriter::token(const Token &t)
	{
		iwrite("<tok>\n");
		++indent_;
		indent();
		os_ << "<orth>" << t.orth_utf8() << "</orth>\n";
		foreach (const Lexeme& l, t.lexemes()) {
			indent();
			os_ << "<lex>";
			os_ << "<base>" << l.lemma_utf8() << "</base>";
			os_ << "<ctag>" << tagset_.tag_to_string(l.tag()) << "</ctag>";
			os_ << "</lex>\n";
		}
		--indent_;
		iwrite("</tok>\n");
	}

	void XcesWriter::sentence(const std::vector<Token *>& v)
	{
		iwrite("<chunk type=\"s\">\n");
		++indent_;
		foreach (const Token* t, v) {
			token(*t);
		}
		--indent_;
		iwrite("</chunk>\n");
	}

	void XcesWriter::paragraph_head()
	{
		indent();
		os_ << "<chunk id=\"ch" << ++cid_ << "\""
			<< " xlink:href=\"unknown\" type=\"tok\">\n";
	}

	void XcesWriter::paragraph(const std::vector<std::vector<Token *> > &v)
	{
		paragraph_head();
		++indent_;
		foreach (const std::vector<Token *>& s, v) {
			sentence(s);
		}
		--indent_;
		iwrite("</chunk>\n");
	}

} /* end ns PlTagger */
