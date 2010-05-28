#include "debug.h"

#include <sstream>

#include <boost/algorithm/string.hpp>

namespace PlTagger {

	std::string lexeme_string(const Lexeme& l)
	{
		std::stringstream ss;
		ss << l.lemma_utf8() << "\t" << l.tag().to_string();
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

} /* end ns PlTagger */
