#ifndef LIBPLTAGGER_MORPHANALYSER_H
#define LIBPLTAGGER_MORPHANALYSER_H

#include <libtoki/token.h>
#include "token.h"
#include "tagset.h"

#include <boost/function.hpp>

namespace PlTagger {

	class MorphAnalyser
	{
	public:
		explicit MorphAnalyser(const Tagset* tagset);

		virtual ~MorphAnalyser();

		std::vector<Token*> process(const Toki::Token& t);

		void process(const Toki::Token &t, std::vector<Token*>& vec);

		virtual void process_functional(const Toki::Token &t, boost::function<void (Token*)> sink) = 0;

		const Tagset& tagset() const {
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
