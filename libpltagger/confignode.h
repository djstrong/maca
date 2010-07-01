#ifndef LIBPLTAGGER_CONFIGNODE_H
#define LIBPLTAGGER_CONFIGNODE_H

#include <libtoki/confignode.h>
#include <libpltagger/exception.h>

namespace PlTagger {

	/// pull libtoki config namespace into our own
	namespace Config = ::Toki::Config;

	/**
	 * Exception class used for signalling missing values in config objects
	 */
	class ConfigValueMissing : public PlTaggerError
	{
	public:
		/// Constructor with an optional description string
		ConfigValueMissing(const std::string& attribute, const std::string& where = "");

		/// Destructor
		~ConfigValueMissing() throw();

		/// PlTaggerError override
		std::string info() const;

		/// the missing item
		std::string attribute;

		/// optional circumstance info
		std::string where;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONFIGNODE_H
