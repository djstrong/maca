#include <libmaca/io/xcescommon.h>
#include <libmaca/io/xceswriter.h>
#include <libtoki/util/foreach.h>

namespace Maca {

	XcesWriter::XcesWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params)
		: TokenWriter(os, tagset, params), cid_(0)
		, use_indent_(true), force_chunk_(false), force_disamb_(false)
		, sort_tags_(false), split_chunks_on_newlines_(false)
	{
		foreach (const string_range& param, params) {
			std::string p = boost::copy_range<std::string>(param);
			if (p == "flat") {
				use_indent_ = false;
			} else if (p == "chunk") {
				force_chunk_ = true;
			} else if (p == "nochunk") {
				force_chunk_ = false;
			} else if (p == "disamb") {
				force_disamb_ = true;
			} else if (p == "sorttags") {
				sort_tags_ = true;
			} else if (p == "split") {
				split_chunks_on_newlines_ = true;
			}
		}
		do_header();
	}

	XcesWriter::~XcesWriter()
	{
		finish();
	}

	void XcesWriter::write_token(const Token &t)
	{
		token_as_xces_xml(os(), tagset(), t, use_indent_ ? indent_level() : -1,
				force_disamb_, sort_tags_);
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
		bool new_chunk = true;
		foreach (const Sentence* s, c.sentences()) {
			if (split_chunks_on_newlines_ && !s->tokens().empty()) {
				const Token* first = s->tokens()[0];
				if (first->wa() == Toki::Whitespace::ManyNewlines) {
					new_chunk = true;
				}
			}
			if (new_chunk) {
				paragraph_head(c);
				if (use_indent_) indent_more();
				new_chunk = false;
			}
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
		if (use_indent_) indent_more();
		if (force_chunk_) {
			paragraph_head();
			if (use_indent_) indent_more();
		}
	}

	void XcesWriter::do_footer()
	{
		if (force_chunk_) {
			if (use_indent_) indent_less();
			osi() << "</chunk>\n";
		}
		if (use_indent_) indent_less();
		os() << "</chunkList>\n";
		os() << "</cesAna>\n";
	}

	void XcesWriter::paragraph_head()
	{
		osi() << "<chunk id=\"ch" << ++cid_ << "\""
			<< " type=\"p\">\n";
	}

	void XcesWriter::paragraph_head(const Chunk& c)
	{
		osi() << "<chunk";
		foreach (const Chunk::attr_map_t::value_type& v, c.attributes()) {
			os() << " " << v.first << "=\"" << v.second << "\"";
		}
		os() << ">\n";
	}

} /* end ns Maca */
