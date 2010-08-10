#include <libpltagger/io/xcescommon.h>
#include <libpltagger/io/xceswriter.h>
#include <libtoki/util/foreach.h>

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

	void XcesWriter::write_token(const Token &t)
	{
		token_as_xces_xml(os(), tagset(), t, use_indent_ ? indent_level() : -1);
	}

	void XcesWriter::write_sentence(const Sentence& s)
	{
		osi() << "<chunk type=\"s\">\n";
		if (use_indent_) indent_more();
		foreach (const Token* t, s.tokens()) {
			write_token(*t);
		}
		if (use_indent_) indent_less();
		osi() << "</chunk>\n";
	}

	void XcesWriter::write_chunk(const Chunk &c)
	{
		paragraph_head(c);
		if (use_indent_) indent_more();
		foreach (const Sentence* s, c.sentences()) {
			write_sentence(*s);
		}
		if (use_indent_) indent_less();
		osi() << "</chunk>\n";
	}

	void XcesWriter::do_header()
	{
		os() << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		os() << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
		os() << "<cesAna";
		os() << " xmlns:xlink=\"http://www.w3.org/1999/xlink\"";
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

	void XcesWriter::paragraph_head()
	{
		osi() << "<chunk id=\"ch" << ++cid_ << "\""
			<< " type=\"tok\">\n";
	}

	void XcesWriter::paragraph_head(const Chunk& c)
	{
		osi() << "<chunk";
		foreach (const Chunk::attr_map_t::value_type& v, c.attributes()) {
			os() << " " << v.first << "=\"" << v.second << "\"";
		}
		os() << ">\n";
	}

} /* end ns PlTagger */
