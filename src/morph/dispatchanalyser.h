#ifndef LIBPLTAGGER_DISPATCHANALYSER_H
#define LIBPLTAGGER_DISPATCHANALYSER_H

#include "morphanalyser.h"

#include <map>
#include <set>

namespace PlTagger {

	class DispatchAnalyser : public MorphAnalyser
	{
	public:
		DispatchAnalyser(const Tagset* tagset);

		~DispatchAnalyser();

		Token* process(const Toki::Token &t);

		void add_type_handler(const std::string& type,  MorphAnalyser* a);

	private:
		std::map<std::string, MorphAnalyser*> type_handlers_;
		std::set<MorphAnalyser*> analysers_;
		MorphAnalyser* default_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_DISPATCHANALYSER_H
