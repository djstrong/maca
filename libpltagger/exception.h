#ifndef PLTAGGER_EXCEPTION_H
#define PLTAGGER_EXCEPTION_H

#include <stdexcept>

namespace PlTagger {

	/**
	 * Base class for all tagger errors. Derives from
	 * @c std::runtime_error. Call member function @c what to get a
	 * human-readable message associated with the error.
	 */
	class PlTaggerError : public std::runtime_error
	{
	public:
		/**
		 * Instantiate a PlTaggerError instance with the given message.
		 * @param what The message to associate with this error.
		 */
		PlTaggerError(const std::string &what);

		~PlTaggerError() throw();

		virtual std::string info() const;
	};

	class FileNotFound : public PlTaggerError
	{
	public:
		FileNotFound(const std::string& filename, const std::string& paths,
				const std::string& where);

		~FileNotFound() throw();

		std::string info() const;

		std::string filename, paths, where;
	};

} /* end ns PlTagger */

#endif // PLTAGGER_EXCEPTION_H
