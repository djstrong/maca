#include <libmaca/util/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libmaca/config_d.h>
#endif


namespace Maca {

	MacaPathSearcher::MacaPathSearcher()
		: Toki::PathSearcher<FileNotFound>(LIBMACA_PATH_SEPARATOR)
	{
#ifdef LIBMACA_DATA_DIR
		set_search_path(LIBMACA_DATA_DIR);
#else
		set_search_path(".");
#endif
	}

	Config::Node get_named_config(const std::string &id)
	{
		std::string fn = Path::Instance().find_file_or_throw(id + ".ini", "analyser config");
		return Config::from_file(fn);
	}

} /* end namespace Maca */
