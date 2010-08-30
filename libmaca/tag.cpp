#include <libmaca/tag.h>
#include <libmaca/tagsetmanager.h>

#include <libtoki/util/foreach.h>

#include <cstring>
#include <sstream>

#include <boost/functional/hash.hpp>

namespace Maca {

	Tag::Tag()
		: pos_id_(-1), tagset_id_(-1)
	{
	}

	Tag::Tag(tagset_idx_t tagset_id, pos_idx_t pos)
		: pos_id_(pos), tagset_id_(tagset_id)
	{
	}

	Tag::Tag(tagset_idx_t tagset_id, pos_idx_t pos, const std::vector<value_idx_t> &values)
		: pos_id_(pos), values_(values), tagset_id_(tagset_id)
	{
	}

	bool Tag::has_valid_tagset() const
	{
		return tagset_id_ != static_cast<tagset_idx_t>(-1)
			&& TagsetManagerSingleton::Instance().get_cache_entry(tagset_id());
	}

	std::string Tag::raw_dump() const
	{
		std::ostringstream ss;
		ss << "[";
		ss << static_cast<int>(tagset_id_) << "#" << static_cast<int>(pos_id_);
		foreach (value_idx_t v, values_) {
			ss << ":" << static_cast<int>(v) ;
		}
		ss << "]";
		return ss.str();
	}

	bool Tag::operator<(const Tag& other) const
	{
		return tagset_id_ < other.tagset_id_
				|| (tagset_id_ == other.tagset_id_
					&& (pos_id_ < other.pos_id_
					|| (pos_id_ == other.pos_id_
						&& (values_.size() < other.values_.size()
						|| (values_.size() == other.values_.size()
							&& memcmp(&values_[0], &other.values_[0],
								std::min(values_.size(), other.values_.size())) < 0)))));
	}

	bool Tag::operator ==(const Tag& other) const
	{
		return tagset_id_ == other.tagset_id_
				&& pos_id_ == other.pos_id_
				&& values_ == other.values_;
	}

	size_t hash_value(const Tag& tag)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, tag.pos_id_);
		boost::hash_combine(seed, tag.tagset_id_);
		boost::hash_combine(seed, tag.values_);
		return seed;
	}

} /* end ns Maca */
