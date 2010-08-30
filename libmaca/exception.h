#ifndef LIBMACA_EXCEPTION_H
#define LIBMACA_EXCEPTION_H

#include <stdexcept>

namespace Maca {

	/**
	 * Base class for all tagger errors. Derives from
	 * @c std::runtime_error. Call member function @c what to get a
	 * human-readable message associated with the error.
	 */
	class MacaError : public std::runtime_error
	{
	public:
		/**
		 * Instantiate a MacaError instance with the given message.
		 * @param what The message to associate with this error.
		 */
		MacaError(const std::string &what);

		~MacaError() throw();

		virtual std::string info() const;
	};

	class FileNotFound : public MacaError
	{
	public:
		FileNotFound(const std::string& filename, const std::string& paths,
				const std::string& where);

		~FileNotFound() throw();

		std::string info() const;

		std::string filename, paths, where;
	};

} /* end ns Maca */

#endif // LIBMACA_EXCEPTION_H
