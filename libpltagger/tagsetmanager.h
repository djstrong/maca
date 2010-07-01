#ifndef LIBPLTAGGER_TAGSETMANAGER_H
#define LIBPLTAGGER_TAGSETMANAGER_H

#include <libpltagger/tagset.h>
#include <libpltagger/exception.h>

#include <boost/shared_ptr.hpp>
#include <loki/Singleton.h>

namespace PlTagger {

	/**
	 * Exception class for signalling cache errors
	 */
	class TagsetNotFound : public PlTaggerError
	{
	public:
		/// Constructor
		explicit TagsetNotFound(const tagset_idx_t id);

		/// Destructor
		~TagsetNotFound() throw() {}

		/// PlTaggerError override
		std::string info() const;

		/// The invalid tagset id
		tagset_idx_t id;
	};

	class TagsetManager : private boost::noncopyable
	{
	public:
		TagsetManager();

		~TagsetManager();

		const Tagset& get_tagset(const std::string& name);

		const Tagset& get_tagset(tagset_idx_t id);

		boost::shared_ptr<Tagset> get_cache_entry(const std::string& name);

		boost::shared_ptr<Tagset> get_cache_entry(tagset_idx_t id);

	private:
		typedef std::map<std::string, boost::shared_ptr<Tagset> > cache_t;
		cache_t cache_;
		typedef std::map<tagset_idx_t, boost::shared_ptr<Tagset> > id_cache_t;
		id_cache_t id_cache_;
	};

	typedef Loki::SingletonHolder< TagsetManager > TagsetManagerSingleton;

	inline const Tagset& get_named_tagset(const std::string& name) {
		return TagsetManagerSingleton::Instance().get_tagset(name);
	}

	inline const Tagset& get_named_tagset(tagset_idx_t id) {
		return TagsetManagerSingleton::Instance().get_tagset(id);
	}

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSETMANAGER_H
