#include <libpltagger/tagsetmanager.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/settings.h>
#include <libtoki/foreach.h>
#include <fstream>

namespace PlTagger {

	TagsetNotFound::TagsetNotFound(const std::string& n)
		: PlTaggerError("Tagset not found")
		, name(n), paths(get_library_config_path_string())
	{
	}

	std::string TagsetNotFound::info() const
	{
		std::ostringstream ss;
		ss << "Tagset '" << name << "' not found in search path " << paths;
		return ss.str();
	}

	TagsetManager::TagsetManager()
		: cache_()
	{
	}

	TagsetManager::~TagsetManager()
	{
	}

	const Tagset& TagsetManager::get_tagset(const std::string &name)
	{
		boost::shared_ptr<Tagset> ptr = get_cache_entry(name);
		if (!ptr) {
			std::ifstream ifs;
			if (!open_file_from_search_path(name + ".tagset", ifs)) {
				throw TagsetNotFound(name);
			}
			ptr.reset(new Tagset);
			*ptr = TagsetParser::load_ini(ifs);
			cache_.insert(std::make_pair(name, ptr));
		}
		return *ptr;
	}

	boost::shared_ptr<Tagset> TagsetManager::get_cache_entry(const std::string &name)
	{
		cache_t::const_iterator i = cache_.find(name);
		if (i == cache_.end()) {
			return boost::shared_ptr<Tagset>();
		} else {
			return i->second;
		}
	}

} /* end ns PlTagger */
