#ifndef LIBPLTAGGER_SETTINGS_H
#define LIBPLTAGGER_SETTINGS_H

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

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SETTINGS_H
