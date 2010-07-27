#ifndef LIBPLTAGGER_MORPH_CREATOR_H
#define LIBPLTAGGER_MORPH_CREATOR_H

#include <libpltagger/morph/morphanalyser.h>
#include <libtoki/confignode.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	MorphAnalyser* make_analyser(const Config::Node& cfg);

	MorphAnalyser* create_analyser(const Config::Node& cfg);

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORPH_CREATOR_H
