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

	Config::Node get_named_config(const std::string &id)
	{
		std::string fn = Path::Instance().find_file_or_throw(id + ".ini", "analyser config");
		return Config::from_file(fn);
	}

} /* end namespace PlTagger */
