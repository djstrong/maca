#ifndef LIBPLTAGGER_SFSTMORPHANALYSER_H
#define LIBPLTAGGER_SFSTMORPHANALYSER_H

#include "morphanalyser.h"
#include "typedefs.h"

class CompactTransducer;

namespace PlTagger {

	class SfstAnalyser : public MorphAnalyser
	{
	public:
		SfstAnalyser(const Tagset* tagset, const std::string& filename);

		~SfstAnalyser();

		Token* process(const Toki::Token& t);

		static std::string unescape_analysis(const std::string& sfst_analysis);

		static Lexeme split_analysis(const std::string& sfst_analysis);

		void split_analysis_into(const std::string &sfst_analysis, std::vector<Lexeme>& lv);

	private:
		CompactTransducer* ct_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SFSTMORPHANALYSER_H
