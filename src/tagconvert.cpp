#include "tagconvert.h"

namespace PlTagger
{
	TagConverter(const Tagset& from, const Tagset& to)
	{
		attr_idx_from_ = from.attribute_dictionary().get_id(attr);
		attr_idx_to_ = to.attribute_dictionary().get_id(attr);
		foreach (value_idx_t v, from.get_attribute_values(attr_idx_from_)) {
			std::string value_name = from.value_dictionary().get_string(v);
			value_idx_t v_to = to.value_dictionary().get_id(value_name);
			mapping_[v] = v_to;
		}
	}

	Tag convert(const Tag& from) const
	{
		pos_map_t::const_iterator pi = pos_mapping_.find(from.pos_id());
		assert(pi != pos_mapping_.end());
		Tag to(tagset_to.id(), pi->second);
		to.values().resize(tagset_to.attribute_dictionary().size());

		foreach (value_idx_t v, from.values()) {
			if (v > 0) {
				std::map<value_idx_t, value_idx_t>::const_iterator i;
				i = mapping_.find(v);
				if (i != mapping_.end()) {
					attribute_idx_t a = tagset_to.get_value_attribute(i->second);
					to.values()[a] = i->second;
				}
			}
		}
		return to;
	}

} /* end ns PlTagger */
