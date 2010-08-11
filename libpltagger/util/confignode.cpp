#include <libpltagger/util/confignode.h>
#include <sstream>

namespace PlTagger {

	ConfigValueMissing::ConfigValueMissing(const std::string& attribute, const std::string& where)
		: PlTaggerError("Value for required attribute '" + attribute + "' missing")
		, attribute(attribute), where(where)
	{
	}

	ConfigValueMissing::~ConfigValueMissing() throw ()
	{
	}

	std::string ConfigValueMissing::info() const
	{
		std::stringstream ss;
		ss << "Value for required config attribute '" << attribute << "' missing";
		if (!where.empty()) {
			ss << " in " << where;
		}
		return ss.str();
	}

} /* end ns PlTagger */
