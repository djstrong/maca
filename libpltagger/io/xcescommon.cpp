#include <libpltagger/io/xcescommon.h>
#include <libtoki/util/foreach.h>

namespace PlTagger {

	namespace {
		std::ostream& osi(std::ostream& os, int indent) {
			for (int i = 0; i < indent; ++i) {
				os << " ";
			}
			return os;
		}
	}

	void token_as_xces_xml(std::ostream& os, const Tagset& tagset, const Token& t, int indent)
	{
		if (t.wa() == Toki::Whitespace::None) {
			osi(os, indent) << "<ns/>\n";
		}
		osi(os, indent) << "<tok>\n";
		++indent;
		osi(os, indent) << "<orth>" << t.orth_utf8() << "</orth>\n";
		foreach (const Lexeme& l, t.lexemes()) {
			osi(os, indent) << "<lex>"
				<< "<base>" << l.lemma_utf8() << "</base>"
				<< "<ctag>" << tagset.tag_to_string(l.tag()) << "</ctag>"
				<< "</lex>\n";
		}
		--indent;
		osi(os, indent) << "</tok>\n";
	}

} /* end ns PlTagger */
