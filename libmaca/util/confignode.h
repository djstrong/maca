#ifndef LIBMACA_CONFIGNODE_H
#define LIBMACA_CONFIGNODE_H

#include <libtoki/util/confignode.h>
#include <libmaca/exception.h>

namespace Maca {

/// pull libtoki config namespace into our own
namespace Config = ::Toki::Config;

/**
 * Exception class used for signalling missing values in config objects
 */
class ConfigValueMissing : public MacaError
{
public:
	/// Constructor with an optional description string
	ConfigValueMissing(const std::string& attribute,
			const std::string& where = "");

	/// Destructor
	~ConfigValueMissing() throw();

	/// MacaError override
	std::string info() const;

	/// the missing item
	std::string attribute;

	/// optional circumstance info
	std::string where;
};

} /* end ns Maca */

#endif // LIBMACA_CONFIGNODE_H
