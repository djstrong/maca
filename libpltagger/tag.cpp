#include <libpltagger/tag.h>
#include <cstring>

namespace PlTagger {

	Tag::Tag()
		: tagset_id_(-1)
	{
	}

	Tag::Tag(const std::string &str)
		: tagset_id_(-1), str_(str)
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

	std::string Tag::to_string() const
	{
		return str_;
	}

	bool Tag::operator<(const Tag& other) const
	{
		return tagset_id_ < other.tagset_id_ || (tagset_id_ == other.tagset_id_
				&& pos_id_ < other.pos_id_ || (pos_id_ == other.pos_id_
				&& values_.size() < other.values_.size() || (values_.size() == other.values_.size()
				&& memcmp(&values_[0], &other.values_[0],
					std::min(values_.size(), other.values_.size())) < 0)));
	}

} /* end ns PlTagger */
