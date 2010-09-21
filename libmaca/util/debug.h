#ifndef LIBMACA_DEBUG_H
#define LIBMACA_DEBUG_H

#include <libcorpus2/token.h>
#include <ostream>

namespace Maca {

std::string lexeme_string(const Lexeme& l);

std::string token_string(const Token& t);

void token_output(const Tagset& tagset, std::ostream& os, Token* t);

} /* end ns Maca */

#endif // LIBMACA_DEBUG_H
