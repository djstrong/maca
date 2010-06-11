#ifndef LIBPLTAGGER_MORPHANALYSER_H
#define LIBPLTAGGER_MORPHANALYSER_H

#include <libtoki/token.h>
#include "token.h"
#include "tagset.h"

namespace PlTagger {

	class MorphAnalyser
	{
	public:
		explicit MorphAnalyser(const Tagset* tagset);

		virtual ~MorphAnalyser();

		virtual std::vector<Token*> process(const Toki::Token& t) = 0;

		const Tagset& tagset() {
			return *tagset_;
		}

		void set_tagset(const Tagset* tagset) {
			tagset_ = tagset;
		}

	private:
		const Tagset* tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORPHANALYSER_H
