#include <libpltagger/sentence.h>
#include <libtoki/util/foreach.h>

namespace PlTagger {

	Sentence::Sentence()
		: tokens_()
	{
	}

	Sentence::~Sentence()
	{
		foreach (Token* t, tokens_) {
			delete t;
		}
	}

} /* end ns PlTagger */
