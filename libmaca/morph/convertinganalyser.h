#ifndef LIBMACA_MORPH_CONVERTINGANALYSER_H
#define LIBMACA_MORPH_CONVERTINGANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/conv/tagsetconverter.h>
#include <boost/scoped_ptr.hpp>

namespace Maca {

/**
 * A wrapper analyser that passes the output of some analyser thorugh a
 * converter.
 *
 * Configuration class key: \b wrap_convert
 */
class ConvertingAnalyser : public MorphAnalyser
{
public:
	/**
	 * Create a converting analyser from an analyser and a converter
	 */
	ConvertingAnalyser(MorphAnalyser* ma, Conversion::TagsetConverter* conv);

	/**
	 * Config node constructor. Recognized keys are:
	 * - wrapped_class - the actual analyser to instantiate and wrap
	 * - wrapped_converter - the converter to use
	 * - wrapped_tagset - tagset of the wrapped analyser -- the 'tagset' key is
	 *                    the tagset of this analyser, so should be equal to
	 *                    the output tagset of the converter. Conversely, the
	 *                    input tagste of the converter should be the wrapped
	 *                    tagset.
	 * Keys are passed to the wrapped analyser, with the tagset substituted
	 * with the wrapped_tagset.
	 */
	explicit ConvertingAnalyser(const Config::Node& cfg);

	/// Cloning
	ConvertingAnalyser* clone() const;

	/// MapAnalyser override
	bool process_functional(const Toki::Token &t
			, boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	/// The wrapped analyser
	boost::scoped_ptr<MorphAnalyser> wrapped_;

	/// The converter object
	boost::scoped_ptr<Conversion::TagsetConverter> converter_;
};

} /* end ns Maca */

#endif // LIBMACA_MORPH_CONVERTINGANALYSER_H
