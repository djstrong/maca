#include <libpltagger/io/xceswriter.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	XcesWriter::XcesWriter(std::ostream& os, const Tagset& tagset, bool force_chunk /*=true*/)
		: TokenWriter(os, tagset), cid_(0), use_indent_(true), force_chunk_(force_chunk)
	{
		do_header();
	}

	XcesWriter* XcesWriter::create_flat(std::ostream& os, const Tagset& tagset)
	{
		XcesWriter* w = new XcesWriter(os, tagset);
		w->use_indent(false);
		return w;
	}

	XcesWriter* XcesWriter::create_nochunk(std::ostream& os, const Tagset& tagset)
	{
		return new XcesWriter(os, tagset, false);
	}

	XcesWriter* XcesWriter::create_flat_nochunk(std::ostream& os, const Tagset& tagset)
	{
		XcesWriter* w = new XcesWriter(os, tagset, false);
		w->use_indent(false);
		return w;
	}

	XcesWriter::~XcesWriter()
	{
		finish();
	}

	void XcesWriter::write_sentence(const std::vector<Token *>& v)
	{
		do_sentence(v);
	}

	void XcesWriter::write_paragraph(const std::vector<std::vector<Token *> > &v)
	{
		paragraph_head();
		if (use_indent_) indent_more();
		do_paragraph(v);
		if (use_indent_) indent_less();
		osi() << "</chunk>\n";
	}

	void XcesWriter::do_header()
	{
		os() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		os() << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
		os() << "<cesAna";
		//os() << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
		os() << " version=\"1.0\" type=\"lex disamb\">\n";
		os() << "<chunkList>\n";
		if (force_chunk_) paragraph_head();
	}

	void XcesWriter::do_footer()
	{
		if (force_chunk_) os() << "</chunk>\n";
		os() << "</chunkList>\n";
		os() << "</cesAna>\n";
	}

	void XcesWriter::do_token(const Token &t)
	{
		if (t.wa() == Toki::Whitespace::None) {
			osi() << "<ns/>\n";
		}
		osi() << "<tok>\n";
		if (use_indent_) indent_more();
		osi() << "<orth>" << t.orth_utf8() << "</orth>\n";
		foreach (const Lexeme& l, t.lexemes()) {
			osi() << "<lex>"
				<< "<base>" << l.lemma_utf8() << "</base>"
				<< "<ctag>" << tagset().tag_to_string(l.tag()) << "</ctag>"
				<< "</lex>\n";
		}
		if (use_indent_) indent_less();
		osi() << "</tok>\n";
	}

	void XcesWriter::do_sentence(const std::vector<Token *>& v)
	{
		osi() << "<chunk type=\"s\">\n";
		if (use_indent_) indent_more();
		foreach (const Token* t, v) {
			do_token(*t);
		}
		if (use_indent_) indent_less();
		osi() << "</chunk>\n";
	}

	void XcesWriter::do_paragraph(const std::vector< std::vector<Token *> >& v)
	{
		TokenWriter::do_paragraph(v);
	}

	void XcesWriter::paragraph_head()
	{
		osi() << "<chunk id=\"ch" << ++cid_ << "\""
			<< " type=\"tok\">\n";
	}

} /* end ns PlTagger */
