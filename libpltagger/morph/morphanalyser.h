#ifndef LIBPLTAGGER_MORPHANALYSER_H
#define LIBPLTAGGER_MORPHANALYSER_H

#include <libtoki/token.h>
#include <libpltagger/confignode.h>
#include <libpltagger/token.h>
#include <libpltagger/tagset.h>

#include <boost/function.hpp>

namespace PlTagger {

	/**
	 * Base class for morphological analysers.
	 *
	 * A MorphAnalyser analyser takes a Toki token and returns one or more
	 * tagger tokens, each of which contains one or more lexemes. An analyser
	 * returns tags from a specified tagset.
	 */
	class MorphAnalyser
	{
	public:
		/// Constructor for a MorphAnalyser working within a given tagset
		explicit MorphAnalyser(const Tagset* tagset);

		/**
		 * Config node constructor. Recognized keys are:
		 * - tagset - the tagset name to load and use
		 */
		explicit MorphAnalyser(const Config::Node& cfg);

		/// Destructor
		virtual ~MorphAnalyser();

		/// Convenience process_functional wrapper to process a Toki token
		/// and return a vector of tagger tokens.
		std::vector<Token*> process(const Toki::Token& t);

		/// Convenience process_functional wrapper  to process a Toki token
		/// and insert the resulting tagger tokens into the given vector.
		void process(const Toki::Token &t, std::vector<Token*>& vec);

		/**
		 * The main token analysis function to be implemented in derived
		 * classes. Takes a Toki token and feeds the resulting tagger tokens
		 * into the sink function object.
		 *
		 * The sink function can be called zero or more times, with zero
		 * indicating that the token has not been recognized at all, and more
		 * than one token meaning that the analyser decided to split the token
		 * into smaller parts.
		 *
		 * Should return true if the sink was called at least once, and false
		 * of no tokens were output.
		 */
		virtual bool process_functional(const Toki::Token &t, boost::function<void (Token*)> sink) = 0;

		/// Tagset accesor
		const Tagset& tagset() const {
			return *tagset_;
		}

		/// Tagset setter
		void set_tagset(const Tagset* tagset) {
			tagset_ = tagset;
		}

		/// Convenience function for adding lexemes to tokens
		void parse_tag_into_token(Token* tok, const UnicodeString& lemma,
				const std::string& tag) const;

	private:
		/// The tagset used by this analyser
		const Tagset* tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORPHANALYSER_H
