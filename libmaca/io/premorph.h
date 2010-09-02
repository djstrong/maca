#ifndef LIBMACA_IO_PREMORPH_H
#define LIBMACA_IO_PREMORPH_H

#include <libmaca/token.h>
#include <libmaca/morph/morphanalyser.h>

#include <libtoki/tokenizer/tokenizer.h>

#include <boost/scoped_ptr.hpp>



namespace Maca {

	class PremorphProcessorImpl;

	class PremorphProcessor
	{
	public:
		PremorphProcessor(std::ostream& os, Toki::Tokenizer& tok, Maca::MorphAnalyser& ma);

		~PremorphProcessor();

		void parse_file(const std::string& filename);

		void parse_stream(std::istream& is);

		void set_stats(bool v);

	protected:
		boost::scoped_ptr<PremorphProcessorImpl> impl_;

		size_t tokens_;

		size_t sentences_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_PREMORPH_H
