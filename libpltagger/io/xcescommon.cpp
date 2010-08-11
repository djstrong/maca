#include <libpltagger/io/xcescommon.h>
#include <libtoki/util/foreach.h>
#include <sstream>

namespace PlTagger {

	namespace {
		std::ostream& osi(std::ostream& os, int indent) {
			for (int i = 0; i < indent; ++i) {
				os << " ";
			}
			return os;
		}
	}

	void token_as_xces_xml(std::ostream& os, const Tagset& tagset, const Token& t
			, int indent, bool force_disamb /* = false */, bool sort /* = false */)
	{
		if (t.wa() == Toki::Whitespace::None) {
			osi(os, indent) << "<ns/>\n";
		}
		osi(os, indent) << "<tok>\n";
		++indent;
		osi(os, indent) << "<orth>" << t.orth_utf8() << "</orth>\n";
		if (!sort) {
			foreach (const Lexeme& l, t.lexemes()) {
				osi(os, indent) << (force_disamb ? "<lex disamb=\"1\">" : "<lex>");
				os << "<base>" << l.lemma_utf8() << "</base>"
					<< "<ctag>" << tagset.tag_to_string(l.tag()) << "</ctag>"
					<< "</lex>\n";
			}
		} else {
			std::stringstream ss;
			std::vector<std::string> vss;
			foreach (const Lexeme& l, t.lexemes()) {
				osi(ss, indent) << (force_disamb ? "<lex disamb=\"1\">" : "<lex>");
				ss << "<base>" << l.lemma_utf8() << "</base>"
					<< "<ctag>" << tagset.tag_to_string(l.tag()) << "</ctag>"
					<< "</lex>\n";
				vss.push_back(ss.str());
				ss.str("");
			}
			std::sort(vss.begin(), vss.end());
			foreach (const std::string& s, vss) {
				os << s;
			}
		}
		--indent;
		osi(os, indent) << "</tok>\n";
	}

} /* end ns PlTagger */
