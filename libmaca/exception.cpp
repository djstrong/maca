#include <libmaca/exception.h>
#include <sstream>
#include <libmaca/util/settings.h>
namespace Maca {

	MacaError::MacaError(const std::string &what)
	 : std::runtime_error(what)
	{
	}

	MacaError::~MacaError() throw()
	{
	}

	std::string MacaError::info() const
	{
		return what();
	}

	FileNotFound::FileNotFound(const std::string& filename, const std::string& paths,
			const std::string& where)
		: MacaError("File not found: " + filename), filename(filename),
		paths(paths), where(where)
	{
	}

	FileNotFound::~FileNotFound() throw()
	{
	}

	std::string FileNotFound::info() const
	{
		std::ostringstream ss;
		if (where.empty()) {
			ss << "File ";
		} else {
			ss << where << " file ";
		}
		ss << "'" << filename << "' not found in search path " << paths;
		return ss.str();
	}

} /* end ns Maca */
