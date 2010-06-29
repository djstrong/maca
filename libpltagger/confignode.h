#ifndef LIBPLTAGGER_CONFIGNODE_H
#define LIBPLTAGGER_CONFIGNODE_H

#include <libtoki/confignode.h>
#include <libpltagger/exception.h>

namespace PlTagger {

	namespace Config = ::Toki::Config;

	class ConfigValueMissing : public PlTaggerError
	{
	public:
		ConfigValueMissing(const std::string& attribute, const std::string& where = "");

		~ConfigValueMissing() throw();

		std::string info() const;

		std::string attribute, where;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONFIGNODE_H
