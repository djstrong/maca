#ifndef LIBMACA_IO_XCESREADER_H
#define LIBMACA_IO_XCESREADER_H

#include <libmaca/io/reader.h>
#include <libmaca/io/xces.h>
#include <libmaca/chunk.h>
#include <deque>
#include <boost/scoped_ptr.hpp>

namespace Maca {

	class XcesReaderImpl;

	class XcesReader : public BufferedTokenReader
	{
	public:
		XcesReader(const Tagset& tagset, std::istream& is,
				bool disamb_only = false);

		~XcesReader();

		std::istream& is() {
			return is_;
		}

	protected:
		void ensure_more();

		std::istream& is_;

		boost::scoped_ptr<XcesReaderImpl> impl_;
	};


} /* end ns Maca */

#endif // LIBMACA_IO_XCESREADER_H
