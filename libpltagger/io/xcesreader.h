#ifndef LIBPLTAGGER_IO_XCESREADER_H
#define LIBPLTAGGER_IO_XCESREADER_H

#include <libpltagger/io/reader.h>
#include <libpltagger/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>

namespace PlTagger {

	class XcesReaderImpl;

	class XcesReaderError : public PlTaggerError
	{
	public:
		/// Constructor
		XcesReaderError(const std::string &what);

		/// Destructor
		~XcesReaderError() throw();
	};

	class XcesReader
	{
	public:
		XcesReader(const Tagset& tagset, std::istream& is, bool disamb_only = false);

		~XcesReader();

		Chunk* get_next_chunk();

	protected:
		std::istream& is_;

		boost::scoped_ptr<XcesReaderImpl> impl_;
	};


} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCESREADER_H
