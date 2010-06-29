#ifndef LIBPLTAGGER_MORFEUSZANALYSER_H
#define LIBPLTAGGER_MORFEUSZANALYSER_H

#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/conv/tagsetconverter.h>

#include <morfeusz.h>

namespace PlTagger {

	class MorfeuszAnalyser : public MorphAnalyser
	{
	public:
		MorfeuszAnalyser(const Tagset* tagset, Conversion::TagsetConverter* conv);

		MorfeuszAnalyser(const Config::Node& cfg);

		void process_functional(const Toki::Token &t, boost::function<void(Token *)>sink);

		Token* make_token(const Toki::Token& t, InterpMorf* im) const;

		void pmorf_into_token(Token* tt,  InterpMorf* im) const;

		void flush_convert(std::vector<Token*>& vec, boost::function<void(Token *)>sink);

		void flush_convert(std::vector< std::vector<Token*> >& vec, boost::function<void(Token *)>sink);

	private:
		Conversion::TagsetConverter* conv_;
	};

} /* end ns Pltagger */

#endif // LIBPLTAGGER_MORFEUSZANALYSER_H
