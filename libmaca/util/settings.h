#ifndef LIBMACA_SETTINGS_H
#define LIBMACA_SETTINGS_H

#include <libmaca/util/confignode.h>
#include <libmaca/exception.h>
#include <libpwrutils/pathsearch.h>
#include <loki/Singleton.h>

namespace Maca {

class MacaPathSearcher : public PwrNlp::PathSearcher<FileNotFound>
{
public:
	MacaPathSearcher();
};

typedef Loki::SingletonHolder<MacaPathSearcher> Path;

/**
 * Get a config from standard directories by name, shorthand function.
 */
Config::Node get_named_config(const std::string& id);

} /* end ns Maca */

#endif // LIBMACA_SETTINGS_H
