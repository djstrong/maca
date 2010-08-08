#ifndef LIBPLTAGGER_IO_XCES_H
#define LIBPLTAGGER_IO_XCES_H

#include <boost/scoped_ptr.hpp>
#include <libpltagger/tagset.h>
#include <libpltagger/tokensource.h>

namespace PlTagger {

	class XcesTokenReaderImpl;

	class XcesTokenReader : public TokenSource
	{
	public:
		XcesTokenReader(const Tagset& tagset, std::istream& is);

		~XcesTokenReader();

		Token* get_next_token();

	protected:
		std::istream& is_;

		boost::scoped_ptr<XcesTokenReaderImpl> impl_;
	};


} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCES_H
