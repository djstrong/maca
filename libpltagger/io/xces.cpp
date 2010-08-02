#include <libpltagger/io/xces.h>
#include <libtoki/foreach.h>
#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>

namespace PlTagger {

	XcesWriter::XcesWriter(std::ostream& os, const Tagset& tagset)
		: TokenWriter(os, tagset), cid_(0)
	{
		do_header();
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
		indent_more();
		do_paragraph(v);
		indent_less();
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
	}

	void XcesWriter::do_footer()
	{
		os() << "</chunkList>\n";
		os() << "</cesAna>\n";
	}

	void XcesWriter::do_token(const Token &t)
	{
		if (t.wa() == Toki::Whitespace::None) {
			osi() << "<ns/>\n";
		}
		osi() << "<tok>\n";
		indent_more();
		osi() << "<orth>" << t.orth_utf8() << "</orth>\n";
		foreach (const Lexeme& l, t.lexemes()) {
			osi() << "<lex>"
				<< "<base>" << l.lemma_utf8() << "</base>"
				<< "<ctag>" << tagset().tag_to_string(l.tag()) << "</ctag>"
				<< "</lex>\n";
		}
		indent_less();
		osi() << "</tok>\n";
	}

	void XcesWriter::do_sentence(const std::vector<Token *>& v)
	{
		osi() << "<chunk type=\"s\">\n";
		indent_more();
		foreach (const Token* t, v) {
			do_token(*t);
		}
		indent_less();
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


	namespace {
		const xmlpp::Node* get_child_or_null(const xmlpp::Node* n, const char* name)
		{
			const xmlpp::Node::NodeList list = n->get_children(name);
			if (list.empty()) return NULL;
			return list.front();
		}

		const xmlpp::Node* get_child_or_throw(const xmlpp::Node* n, const char* name)
		{
			const xmlpp::Node* c = get_child_or_null(n, name);
			if (!c) {
				std::stringstream ss;
				ss << "no " << "<" << name << ">";
				throw PlTaggerError(ss.str());
			}
			return c;
		}

		std::string get_child_text_or_empty(const xmlpp::Node* n, const char* name)
		{
			const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(
				get_child_or_null(n, name));
			if (el) {
				const xmlpp::TextNode* t = el->get_child_text();
				if (t) return t->get_content();
			}
			return "";
		}
	} /* end anon ns */

	XcesReader::XcesReader(std::istream& is, const Tagset& tagset)
		: TokenReader(is, tagset)
	{
		xmlpp::DomParser dom;
		dom.set_substitute_entities();
		try {
			dom.parse_stream(is);
		} catch (xmlpp::parse_error& e) {
			std::stringstream ss;
			ss << "XCES XML parse error : " << e.what();
			if (e.what() == std::string("libxml error 201")) {
				ss << " (maybe the XML is missing a "
					<< "xmlns:xlink=\"http://www.w3.org/1999/xlink\""
					<< " in the root node?)";
			}
			throw PlTaggerError(ss.str());
		}

		if (dom) {
			const xmlpp::Node* root = dom.get_document()->get_root_node();
			if (!root) throw PlTaggerError("no root");
			if (root->get_name() != "cesAna") throw PlTaggerError("root node not <cesAna>");
			const xmlpp::Node* chunklist = get_child_or_throw(root, "chunkList");
			const xmlpp::Node::NodeList chunks = chunklist->get_children("chunk");
			foreach (const xmlpp::Node* c, chunks) {
				const xmlpp::Node::NodeList sentences = c->get_children("chunk");
				paragraphs_.resize(paragraphs_.size() + 1);
				foreach (const xmlpp::Node* s, sentences) {
					const xmlpp::Node::NodeList tokens = s->get_children("tok");
					paragraphs_.back().resize(paragraphs_.back().size() + 1);
					foreach (const xmlpp::Node* t, tokens) {
						Token* tok = token_from_tok_node(t);
						if (tok) {
							paragraphs_.back().back().push_back(tok);
						}
					}
				}
			}
		}
	}

	XcesReader::~XcesReader()
	{
	}

	Token* XcesReader::token_from_tok_node(const xmlpp::Node *n)
	{
		const xmlpp::Element* el = dynamic_cast<const xmlpp::Element*>(n);
		if (!el) throw PlTaggerError("<tok> not an Element");
		std::string orth_utf8 = get_child_text_or_empty(n, "orth");
		Token* t = new Token(UnicodeString::fromUTF8(orth_utf8), Toki::Whitespace::Space);
		const xmlpp::Node::NodeList lexemes = n->get_children("lex");
		foreach (const xmlpp::Node* l, lexemes) {
			std::string lemma_utf8 = get_child_text_or_empty(l, "base");
			std::string tag_string = get_child_text_or_empty(l, "ctag");
			Tag tag = tagset().parse_simple_tag(tag_string, false);
			Lexeme lex(UnicodeString::fromUTF8(lemma_utf8), tag);
			t->add_lexeme(lex);
		}
		return t;
	}

	std::vector< std::vector<Token*> > XcesReader::read_paragraph()
	{
		if (paragraphs_.empty()) {
			return TokenReader::read_paragraph();
		}
		return paragraphs_[0];
	}

} /* end ns PlTagger */
