#include <libpltagger/tagsetmanager.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/settings.h>
#include <libtoki/foreach.h>
#include <fstream>
#include <iostream>

namespace PlTagger {

	TagsetNotFound::TagsetNotFound(tagset_idx_t id)
		: PlTaggerError("Tagset not found")
		, id(id)
	{
	}
	std::string TagsetNotFound::info() const
	{
		std::ostringstream ss;
		ss << "Tagset with id " << (int)id << " not found in cache";
		return ss.str();
	}

	TagsetManager::TagsetManager()
		: cache_(), id_cache_()
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
				throw FileNotFound(name + ".tagset", "Tagset");
			}
			ptr.reset(new Tagset);
			*ptr = TagsetParser::load_ini(ifs);
			cache_.insert(std::make_pair(name, ptr));
			id_cache_.insert(std::make_pair(ptr->id(), ptr));
			ptr->set_name(name);
			std::cerr << "Tagset '" << name << "' loaded with id " << (int)ptr->id() << "\n";
		}
		return *ptr;
	}

	const Tagset& TagsetManager::get_tagset(tagset_idx_t id)
	{
		boost::shared_ptr<Tagset> ptr = get_cache_entry(id);
		if (!ptr) {
			throw TagsetNotFound(id);
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

	boost::shared_ptr<Tagset> TagsetManager::get_cache_entry(tagset_idx_t id)
	{
		id_cache_t::const_iterator i = id_cache_.find(id);
		if (i == id_cache_.end()) {
			return boost::shared_ptr<Tagset>();
		} else {
			return i->second;
		}
	}

} /* end ns PlTagger */
