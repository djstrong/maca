#ifndef LIBMACA_IO_XCESREADER_H
#define LIBMACA_IO_XCESREADER_H

#include <libmaca/io/reader.h>
#include <libmaca/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>

namespace Maca {

	class XcesReaderImpl;

	class XcesReaderError : public MacaError
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


} /* end ns Maca */

#endif // LIBMACA_IO_XCESREADER_H
