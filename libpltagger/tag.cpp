#include <libpltagger/tag.h>
#include <libtoki/foreach.h>
#include <cstring>
#include <sstream>

namespace PlTagger {

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

} /* end ns PlTagger */
