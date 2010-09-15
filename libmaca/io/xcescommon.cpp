#include <libmaca/io/xcescommon.h>
#include <libtoki/util/foreach.h>
#include <sstream>

namespace Maca {

	namespace {
		std::ostream& osi(std::ostream& os, int indent) {
			for (int i = 0; i < indent; ++i) {
				os << " ";
			}
			return os;
		}

		inline void lexeme_as_xces_xml(std::ostream& os, const Tagset& tagset,
				const Lexeme& l, bool output_disamb)
		{
			if (output_disamb && l.is_disamb()) {
				os << "<lex disamb=\"1\">";
			} else {
				os << "<lex>";
			}
			os << "<base>";
			encode_xml_entities_into(os, l.lemma_utf8());
			os << "</base>";
			os << "<ctag>";
			encode_xml_entities_into(os, tagset.tag_to_string(l.tag()));
			os << "</ctag>";
			os << "</lex>\n";
		}
	}

	void token_as_xces_xml(std::ostream& os, const Tagset& tagset,
			const Token& t, int indent, bool output_disamb /* = false */,
			bool sort /* = false */, bool whitespace_info /* false */)
	{
		if (t.wa() == Toki::Whitespace::None) {
			osi(os, indent) << "<ns/>\n";
		}
		if (!whitespace_info) {
			osi(os, indent) << "<tok>\n";
		} else {
			osi(os, indent) << "<tok ws=\""
				<< Toki::Whitespace::to_string(t.wa()) << "\">\n";
		}
		++indent;
		osi(os, indent) << "<orth>";
		encode_xml_entities_into(os, t.orth_utf8());
		os << "</orth>\n";
		if (!sort) {
			foreach (const Lexeme& l, t.lexemes()) {
				lexeme_as_xces_xml(osi(os, indent), tagset, l, output_disamb);
			}
		} else {
			std::stringstream ss;
			std::vector<std::string> vss;
			foreach (const Lexeme& l, t.lexemes()) {
				lexeme_as_xces_xml(osi(ss, indent), tagset, l, output_disamb);
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

	void encode_xml_entities_into(std::ostream& buf, const std::string& input)
	{
		size_t len = input.size();
		for(size_t pos = 0; pos < len; pos++)
		{
			const char &c = input[pos];
			switch(c) {
			case '<':
				buf << "&lt;";
				break;
			case '>':
				buf << "&gt;";
				break;
			case '&':
				buf << "&amp;";
				break;
			case '"':
				buf << "&quot;";
				break;
			case '\'':
				buf << "&apos;";
				break;
			default:
				buf << c;
			}
		}
	}

	void encode_xml_entities(std::string &input)
	{
		size_t len = input.size();
		size_t pos;
		for (pos = 0; pos < len; pos++) {
			const char &c = input[pos];
			if(c == '<'
				|| c == '>'
				|| c == '&'
				|| c == '"'
				|| c == '\'')
			break;
		}

		if (pos < len) {
			std::ostringstream buf;
			buf << input.substr(0, pos);
			for (; pos < len; pos++)
			{
				const char &c = input[pos];
				switch(c) {
				case '<':
					buf << "&lt;";
					break;
				case '>':
					buf << "&gt;";
					break;
				case '&':
					buf << "&amp;";
					break;
				case '"':
					buf << "&quot;";
					break;
				case '\'':
					buf << "&apos;";
					break;
				default:
					buf << c;
				}
			}
			input = buf.str();
		}
	} // EncodeEntities

} /* end ns Maca */
