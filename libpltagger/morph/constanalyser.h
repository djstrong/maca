#ifndef LIBPLTAGGER_CONSTANALYSER_H
#define LIBPLTAGGER_CONSTANALYSER_H

#include <libpltagger/morph/morphanalyser.h>

namespace PlTagger {

	class ConstAnalyser : public MorphAnalyser
	{
	public:
		ConstAnalyser(const Tagset* tagset, const std::string& tag);

		ConstAnalyser(const Tagset* tagset, const Tag& tag);

		explicit ConstAnalyser(const Toki::Config::Node& cfg);

		~ConstAnalyser();

		 void process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);
	private:
		Tag tag_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONSTANALYSER_H
