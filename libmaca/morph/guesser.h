#ifndef LIBMACA_MORPH_GUESSER_H
#define LIBMACA_MORPH_GUESSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/typedefs.h>
#include <boost/thread/mutex.hpp>

namespace Maca {

/**
 * Corpuslib guesser analyser interface.
 *
 * WARNING: Guesser might call morfeusz and end up with ambiguous segmentation,
 *          this is not handled and will result in damaged output. The Morfeusz
 *          analyser should be always used before teh Guesser analyser.
 * WARNING: There is no thread safety currently
 */
class GuesserAnalyser : public MorphAnalyser
{
public:
	/**
	 * Constructor for a Guesser analyser with the given tagset
	 */
	GuesserAnalyser(const Corpus2::Tagset* tagset);

	/**
	 * Config node constructor. No new recognized keys.
	 */
	GuesserAnalyser(const Config::Node& cfg);

	/// Cloning
	GuesserAnalyser* clone() const;

	/// Destructor
	~GuesserAnalyser();

	/// MorphAnalyser override
	bool process_functional(const Toki::Token& t,
			boost::function<void (Corpus2::Token*)> sink);

	/// Class identifier
	static const char* identifier;

	/// Registered flag
	static bool registered;
private:
	/// Set libcorpus1 flags necessary for guesser usage.
	void setup_corpus1();
};

} /* end ns Maca */

#endif // LIBMACA_MORPH_GUESSER_H
