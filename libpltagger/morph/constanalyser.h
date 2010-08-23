#ifndef LIBPLTAGGER_CONSTANALYSER_H
#define LIBPLTAGGER_CONSTANALYSER_H

#include <libpltagger/morph/morphanalyser.h>

namespace PlTagger {

	/**
	 * A simple analyser that always returns the same interpretation for any
	 * token. The tag is pre-set, and the lemma is the token's orth.
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
		 * - lower_lemma - if true, lowercase the lemma (false by default)
		 */
		explicit ConstAnalyser(const Config::Node& cfg);

		/// MapAnalyser override
		bool process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);

		/// Class identifier
		static const char* identifier;

		/// Registered flag
		static bool registered;
	private:
		/// The tag
		Tag tag_;

		/// flag to lowercase lemma
		bool lower_lemma_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONSTANALYSER_H
