#ifndef LIBMACA_MORFEUSZANALYSER_H
#define LIBMACA_MORFEUSZANALYSER_H

#include <libmaca/morph/morphanalyser.h>
#include <libmaca/conv/tagsetconverter.h>

#include <morfeusz.h>

namespace Maca {

	/**
	 * Exception class for signalling Morfeusz-related errors
	 */
	class MorfeuszError : public MacaError
	{
	public:
		/// Constructor
		MorfeuszError(const std::string& error, const std::string input = "",
				InterpMorf* interp = NULL);

		/// Destructor
		~MorfeuszError() throw();

		/// Info accesor
		std::string info() const;

		/// the error info and Morfeusz input during th error, if available
		std::string error, input;

		/// The structure returned by Morfeusz during the error, if available
		InterpMorf* interp;
	};

	/**
	 * A morphological analyser class that interfaces with the Morfeusz library.
	 *
	 * Morfeusz requires a tagset converter since it sometimes returns ambiguous
	 * token segmentation. The converter should modify tokens coming from
	 * Morfeusz so that an unambiguous segmentation is possible.
	 */
	class MorfeuszAnalyser : public MorphAnalyser
	{
	public:
		/**
		 * Constructor for a Morfeusz analyser with a given tagset and converter.
		 * The tagset should be the output tagset of the converter.
		 */
		MorfeuszAnalyser(const Tagset* tagset, Conversion::TagsetConverter* conv);

		/**
		 * Config node constructor. Recognized keys are:
		 * - converter - the converter to load (from standard paths)
		 */
		MorfeuszAnalyser(const Config::Node& cfg);

		/// Destructor
		~MorfeuszAnalyser();

		/// Cloning
		MorfeuszAnalyser* clone() const;

		/// MorphAnalyser override
		bool process_functional(const Toki::Token &t, boost::function<void(Token *)>sink);

		/// helper to create a token from a Morfeusz interpretation struct
		Token* make_token(const Toki::Token& t, InterpMorf* im) const;

		/// helper to add lexemes from a Morfeusz interp struct into a token
		void pmorf_into_token(Token* tt,  InterpMorf* im) const;

		/// convert gathered tokens and pass them to the sink
		void flush_convert(std::vector<Token*>& vec, boost::function<void(Token *)>sink);

		/// convert gethered tokens (ambiguously segmented), try folding and
		/// pass the resulting tokens to the sink
		void flush_convert(std::vector< std::vector<Token*> >& vec, boost::function<void(Token *)>sink);

		/// Class identifier
		static const char* identifier;

		/// Registered flag
		static bool registered;

	private:
		/// the tagset converter
		Conversion::TagsetConverter* conv_;

		Tag ign_tag_;

		bool warn_on_ign_;

		bool warn_on_fold_failure_;
	};

} /* end ns Maca */

#endif // LIBMACA_MORFEUSZANALYSER_H
