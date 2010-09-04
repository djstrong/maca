#ifndef LIBMACA_TAGSETMANAGER_H
#define LIBMACA_TAGSETMANAGER_H

#include <libmaca/tagset.h>
#include <libmaca/exception.h>

#include <boost/shared_ptr.hpp>
#include <loki/Singleton.h>

namespace Maca {

	/**
	 * Exception class for signalling cache errors
	 */
	class TagsetNotFound : public MacaError
	{
	public:
		/// Constructor
		explicit TagsetNotFound(const tagset_idx_t id);

		/// Destructor
		~TagsetNotFound() throw() {}

		/// MacaError override
		std::string info() const;

		/// The invalid tagset id
		tagset_idx_t id;
	};

	/**
	 * Singleton class for accesing tagsets by name that will load tagsets
	 * from the data dir and cache them for future acceses.
	 * @todo this should be a generic utility class
	 */
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

} /* end ns Maca */

#endif // LIBMACA_TAGSETMANAGER_H
