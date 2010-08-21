#include <libpltagger/exception.h>
#include <sstream>
#include <libpltagger/settings.h>
namespace PlTagger {

	PlTaggerError::PlTaggerError(const std::string &what)
	 : std::runtime_error(what)
	{
	}

	PlTaggerError::~PlTaggerError() throw()
	{
	}

	std::string PlTaggerError::info() const
	{
		return what();
	}

	FileNotFound::FileNotFound(const std::string& filename, const std::string& paths,
			const std::string& where)
		: PlTaggerError("File not found: " + filename), filename(filename),
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

} /* end ns PlTagger */
