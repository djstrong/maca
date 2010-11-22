#ifndef LIBMACA_CONSTANALYSER_H
#define LIBMACA_CONSTANALYSER_H

#include <libmaca/morph/morphanalyser.h>

namespace Maca {

/**
 * A simple analyser that always returns the same interpretation for any
 * token. The tag is pre-set, and the lemma is the token's orth.
 *
 * Configuration class key: \b const
 */
class ConstAnalyser : public MorphAnalyser
{
public:
	/// Constructor for a ConstAnalyser with a tagset and a tag string
	ConstAnalyser(const Corpus2::Tagset* tagset, const std::string& tag);

	/// Constructor for a ConstAnalyser with a tagset and a tag string
	ConstAnalyser(const Corpus2::Tagset* tagset, const Corpus2::Tag& tag);

	/**
	 * Config node constructor. Recognized keys are:
	 * - tag - the tag to use as the analysis for all tokens
	 * - lower_lemma - if true, lowercase the lemma (false by default)
	 */
	explicit ConstAnalyser(const Config::Node& cfg);

	/// Cloning
	ConstAnalyser* clone() const;

	/// MapAnalyser override
	bool process_functional(const Toki::Token &t
			, boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	/// The tag
	Corpus2::Tag tag_;

	/// flag to lowercase lemma
	bool lower_lemma_;
};

} /* end ns Maca */

#endif // LIBMACA_CONSTANALYSER_H
