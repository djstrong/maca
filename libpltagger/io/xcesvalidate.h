#ifndef LIBPLTAGGER_IO_XCESVALIDATE_H
#define LIBPLTAGGER_IO_XCESVALIDATE_H

#include <boost/scoped_ptr.hpp>
#include <libpltagger/tagset.h>
#include <libpltagger/tokensource.h>

namespace PlTagger {

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
} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCESVALIDATE_H
