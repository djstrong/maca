#ifndef LIBPLTAGGER_SETTINGS_H
#define LIBPLTAGGER_SETTINGS_H

#include <libpltagger/util/confignode.h>
#include <libpltagger/exception.h>
#include <libtoki/util/pathsearch.h>
#include <loki/Singleton.h>

namespace PlTagger {

	class PlTaggerPathSearcher : public Toki::PathSearcher<FileNotFound>
	{
	public:
		PlTaggerPathSearcher();
	};

	typedef Loki::SingletonHolder<PlTaggerPathSearcher> Path;

	/**
	 * Get a config from standard directories by name, shorthand function.
	 */
	Config::Node get_named_config(const std::string& id);

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SETTINGS_H
