#include <libmaca/util/debug.h>

#include <sstream>

#include <boost/algorithm/string.hpp>

namespace Maca {

	std::string lexeme_string(const Lexeme& l)
	{
		std::stringstream ss;
		ss << l.lemma_utf8() << "\t" << l.tag().raw_dump();
		return ss.str();
	}

	std::string token_string(const Token& t)
	{
		std::stringstream ss;
		ss << t.orth_utf8() << "\t";
		ss << "";
		for (size_t i = 0; i < t.lexemes().size(); ++i) {
			if (i > 0) {
				ss << "\n\t";
			}
			ss << lexeme_string(t.lexemes()[i]);
		}
		ss << "";
		return ss.str();
	}

	void token_output(const Tagset& tagset, std:: ostream& os, Token* t)
	{
		os << (int)tagset.id() << "#" << t->orth_utf8() << "";
		os << "";
		for (size_t i = 0; i < t->lexemes().size(); ++i) {
			//if (i > 0) {
				os << "\n\t";
			//}
			const Lexeme& lex = t->lexemes()[i];
			os << lex.lemma_utf8();
			os << " ";
			os << tagset.tag_to_string(lex.tag());
			os << " ";
			//os << lex.tag().raw_dump();
		}
	}

} /* end ns Maca */
