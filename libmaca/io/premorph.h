#ifndef LIBMACA_IO_PREMORPH_H
#define LIBMACA_IO_PREMORPH_H

#include <libmaca/token.h>
#include <libmaca/io/reader.h>
#include <libmaca/morph/morphanalyser.h>

#include <libtoki/tokenizer/tokenizer.h>

#include <boost/scoped_ptr.hpp>



namespace Maca {

	class PremorphProcessorImpl;

	class SentenceAnalyser;

	class PremorphProcessor
	{
	public:
		PremorphProcessor(std::ostream& os,
				const boost::shared_ptr<Toki::Tokenizer>& tok,
				const boost::shared_ptr<Maca::MorphAnalyser>& ma);

		PremorphProcessor(std::ostream& os,
				const boost::shared_ptr<SentenceAnalyser>& sa);

		~PremorphProcessor();

		void parse_file(const std::string& filename);

		void parse_stream(std::istream& is);

		void set_stats(bool v);

	protected:
		boost::scoped_ptr<PremorphProcessorImpl> impl_;

		size_t tokens_;

		size_t sentences_;
	};

	class PremorphReaderImpl;

	class PremorphReader : public BufferedTokenReader
	{
	public:
		PremorphReader(std::istream& is,
				const boost::shared_ptr<Toki::Tokenizer>& tok,
				const boost::shared_ptr<Maca::MorphAnalyser>& ma);

		PremorphReader(std::istream& is,
				const boost::shared_ptr<SentenceAnalyser>& sa);

		~PremorphReader();

	protected:
		void ensure_more();

		boost::scoped_ptr<PremorphReaderImpl> impl_;
	};

} /* end ns Maca */

#endif // LIBMACA_IO_PREMORPH_H
