#ifndef LIBPLTAGGER_CONSTANALYSER_H
#define LIBPLTAGGER_CONSTANALYSER_H

#include <libpltagger/morph/morphanalyser.h>

namespace PlTagger {

	/**
	 * A simple analyser that always returns the same interpretation for any
	 * token.
	 */
	class ConstAnalyser : public MorphAnalyser
	{
	public:
		/// Constructor for a ConstAnalyser with a tagset and a tag string
		ConstAnalyser(const Tagset* tagset, const std::string& tag);

		/// Constructor for a ConstAnalyser with a tagset and a tag string
		ConstAnalyser(const Tagset* tagset, const Tag& tag);

		/**
		 * Config node constructor. Recognized keys are:
		 * - tag - the tag to use as the analysis for all tokens
		 */
		explicit ConstAnalyser(const Config::Node& cfg);

		/// MapAnalyser override
		void process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);
	private:
		Tag tag_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONSTANALYSER_H
