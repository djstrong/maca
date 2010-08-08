#ifndef LIBPLTAGGER_IO_XCES_H
#define LIBPLTAGGER_IO_XCES_H

#include <boost/scoped_ptr.hpp>
#include <libtoki/tokensource.h>

namespace PlTagger {

	class XcesNoMorphReaderImpl;

	class XcesNoMorphReader// : public Toki::TokenSource
	{
	public:
		XcesNoMorphReader();

		~XcesNoMorphReader();

		void parse_file(const std::string& filename);

		void parse_stream(std::istream& is);

	protected:
		boost::scoped_ptr<XcesNoMorphReaderImpl> impl_;
	};


} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCES_H
