#ifndef LIBPLTAGGER_IO_PREMORPH_H
#define LIBPLTAGGER_IO_PREMORPH_H

#include <libpltagger/token.h>
#include <libpltagger/morph/morphanalyser.h>

#include <libtoki/tokenizer.h>

#include <boost/scoped_ptr.hpp>



namespace PlTagger {

	class PremorphProcessorImpl;

	class PremorphProcessor
	{
	public:
		PremorphProcessor(std::ostream& os, Toki::Tokenizer& tok, PlTagger::MorphAnalyser& ma);

		void process_stream(std::istream& is);

		~PremorphProcessor();

		void parse_file(const std::string& filename);

		void parse_stream(std::istream& is);

	protected:
		boost::scoped_ptr<PremorphProcessorImpl> impl_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_PREMORPH_H
