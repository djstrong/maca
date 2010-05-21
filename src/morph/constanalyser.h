#ifndef LIBPLTAGGER_CONSTANALYSER_H
#define LIBPLTAGGER_CONSTANALYSER_H

#include "morphanalyser.h"

namespace PlTagger {

	class ConstAnalyser : public MorphAnalyser
	{
	public:
		ConstAnalyser(const std::string& tag);

		~ConstAnalyser();

		Token* process(const Toki::Token &t);
	private:
		std::string tag_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONSTANALYSER_H
