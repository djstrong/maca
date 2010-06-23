#ifndef LIBPLTAGGER_TAGSETMANAGER_H
#define LIBPLTAGGER_TAGSETMANAGER_H

#include <libpltagger/tagset.h>
#include <libpltagger/exception.h>

#include <boost/shared_ptr.hpp>
#include <loki/Singleton.h>

namespace PlTagger {

	class TagsetNotFound : public PlTaggerError
	{
	public:
		TagsetNotFound(const std::string& n);

		~TagsetNotFound() throw() {}

		std::string info() const;

		std::string name;

		std::string paths;
	};

	class TagsetManager : private boost::noncopyable
	{
	public:
		TagsetManager();

		~TagsetManager();

		const Tagset& get_tagset(const std::string& name);

		boost::shared_ptr<Tagset> get_cache_entry(const std::string& name);

	private:
		typedef std::map<std::string, boost::shared_ptr<Tagset> > cache_t;
		cache_t cache_;
	};

	typedef Loki::SingletonHolder< TagsetManager > TagsetManagerSingleton;

	inline const Tagset& get_named_tagset(const std::string& name) {
		return TagsetManagerSingleton::Instance().get_tagset(name);
	}


} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSETMANAGER_H
