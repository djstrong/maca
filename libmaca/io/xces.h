#ifndef LIBMACA_IO_XCES_H
#define LIBMACA_IO_XCES_H

#include <libmaca/tagset.h>
#include <libmaca/tokensource.h>

namespace Maca {

	class XcesError : public MacaError
	{
	public:
		/// Constructor
		XcesError(const std::string &what);

		/// Destructor
		~XcesError() throw();
	};

} /* end ns Maca */

#endif // LIBMACA_IO_XCES_H
