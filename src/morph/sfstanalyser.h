#ifndef LIBPLTAGGER_SFSTMORPHANALYSER_H
#define LIBPLTAGGER_SFSTMORPHANALYSER_H

#include "morphanalyser.h"

class CompactTransducer;

namespace PlTagger {

	class SfstAnalyser : public MorphAnalyser
	{
	public:
		SfstAnalyser(const std::string& filename);

		~SfstAnalyser();

		Token* process(const Toki::Token& t);
	private:
		CompactTransducer* ct_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SFSTMORPHANALYSER_H
