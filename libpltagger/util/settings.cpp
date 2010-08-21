#include <libpltagger/util/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif


namespace PlTagger {

	PlTaggerPathSearcher::PlTaggerPathSearcher()
		: Toki::PathSearcher<FileNotFound>(LIBPLTAGGER_PATH_SEPARATOR)
	{
#ifdef LIBPLTAGGER_DATA_DIR
		set_search_path(LIBPLTAGGER_DATA_DIR);
#else
		set_search_path(".");
#endif
	}

} /* end namespace PlTagger */
