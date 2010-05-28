#ifndef LIBPLTAGGER_DISPATCHANALYSER_H
#define LIBPLTAGGER_DISPATCHANALYSER_H

#include "morphanalyser.h"

#include <map>

namespace PlTagger {

	class DispatchAnalyser : public MorphAnalyser
	{
	public:
		DispatchAnalyser();

		~DispatchAnalyser();

		Token* process(const Toki::Token &t);

	private:
		std::map<std::string, MorphAnalyser*> type_handlers_;
		std::vector<MorphAnalyser*> analysers_;
		MorphAnalyser* default_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_DISPATCHANALYSER_H
