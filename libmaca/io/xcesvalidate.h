#ifndef LIBMACA_IO_XCESVALIDATE_H
#define LIBMACA_IO_XCESVALIDATE_H

#include <boost/scoped_ptr.hpp>
#include <libmaca/tagset.h>
#include <libmaca/tokensource.h>

namespace Maca {

	class XcesValidatorImpl;

	class XcesValidator
	{
	public:
		XcesValidator(const Tagset& tagset, std::ostream& out);

		~XcesValidator();

		void validate_stream(std::istream& is);

		void validate_file(const std::string& filename);

	protected:
		boost::scoped_ptr<XcesValidatorImpl> impl_;
	};
} /* end ns Maca */

#endif // LIBMACA_IO_XCESVALIDATE_H
