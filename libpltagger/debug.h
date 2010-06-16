#ifndef PLTAGGER_DEBUG_H
#define PLTAGGER_DEBUG_H

#include <libpltagger/token.h>

namespace PlTagger {

	std::string lexeme_string(const Lexeme& l);

	std::string token_string(const Token& t);

} /* end ns PlTagger */

#endif // PLTAGGER_DEBUG_H
