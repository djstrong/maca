#ifndef LIBMACA_DEBUG_H
#define LIBMACA_DEBUG_H

#include <libcorpus2/token.h>
#include <ostream>

namespace Maca {

std::string lexeme_string(const Corpus2::Lexeme& l);

std::string token_string(const Corpus2::Token& t);

void token_output(const Corpus2::Tagset& tagset, std::ostream& os, Corpus2::Token* t);

} /* end ns Maca */

#endif // LIBMACA_DEBUG_H
