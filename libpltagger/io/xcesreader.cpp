#include <libpltagger/io/xcesreader.h>
#include <libtoki/util/xmlutil.h>
#include <libtoki/util/foreach.h>
#include <libxml++/libxml++.h>

namespace PlTagger {

	using Toki::XmlUtil::get_child_or_throw;
	using Toki::XmlUtil::get_child_text_or_empty;

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
			const xmlpp::Node* chunklist = get_child_or_throw<PlTaggerError>(root, "chunkList");
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
