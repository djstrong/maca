#ifndef LIBPLTAGGER_SFSTMORPHANALYSER_H
#define LIBPLTAGGER_SFSTMORPHANALYSER_H

#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/typedefs.h>

class CompactTransducer;

namespace PlTagger {

	class SfstAnalyser : public MorphAnalyser
	{
	public:
		SfstAnalyser(const Tagset* tagset, const std::string& filename);

		~SfstAnalyser();

		void process_functional(const Toki::Token& t, boost::function<void (Token*)> sink);

		static std::string unescape_analysis(const std::string& sfst_analysis);

		static void split_analysis_into(const std::string &sfst_analysis,
				std::vector<Lexeme>& lv, const Tagset& tagset);

	private:
		CompactTransducer* ct_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SFSTMORPHANALYSER_H
