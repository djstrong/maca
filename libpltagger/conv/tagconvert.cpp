#include <libpltagger/conv/tagconvert.h>
#include <libtoki/foreach.h>

namespace PlTagger
{
	TagConverter::TagConverter(const Tagset& from, const Tagset& to)
		: tagset_from_(from), tagset_to_(to)
	{
		from.pos_dictionary().create_mapping_to(to.pos_dictionary(), pos_mapping_);
		from.attribute_dictionary().create_mapping_to(to.attribute_dictionary(), attribute_mapping_);
		from.value_dictionary().create_mapping_to(to.value_dictionary(), value_mapping_);
	}

	Tag TagConverter::cast(const Tag& from) const
	{
		pos_map_t::const_iterator pi = pos_mapping_.find(from.pos_id());
		assert(pi != pos_mapping_.end());
		Tag to(tagset_to_.id(), pi->second);
		to.values().resize(tagset_to_.attribute_dictionary().size());

		foreach (value_idx_t v, from.values()) {
			if (v > 0) {
				std::map<value_idx_t, value_idx_t>::const_iterator i;
				i = value_mapping_.find(v);
				if (i != value_mapping_.end()) {
					attribute_idx_t a = tagset_to_.get_value_attribute(i->second);
					to.values()[a] = i->second;
				}
			}
		}
		return to;
	}

	TagPredicate::TagPredicate(const std::string& name, const Tagset& tagset)
	{
		second = tagset.value_dictionary().get_id(name);
		if (tagset.value_dictionary().is_id_valid(static_cast<value_idx_t>(second))) {
			first = tagset.get_value_attribute(static_cast<value_idx_t>(second));
		} else {
			first = tagset.attribute_dictionary().get_id(name);
			if (tagset.attribute_dictionary().is_id_valid(static_cast<attribute_idx_t>(first))) {
				second = 0;
			} else {
				first = static_cast<idx_t>(-1);
				second = tagset.pos_dictionary().get_id(name);
				assert(tagset.pos_dictionary().is_id_valid(static_cast<pos_idx_t>(second)));
			}
		}
	}

	TagPredicate::~TagPredicate()
	{
	}

	void TagPredicate::apply(Tag &tag) const
	{
		if (first != static_cast<idx_t>(-1)) {
			tag.values()[first] = static_cast<value_idx_t>(second);
		} else {
			tag.set_pos_id(static_cast<pos_idx_t>(second));
		}
	}

	bool TagPredicate::check(const Tag &tag) const
	{
		if (first != static_cast<idx_t>(-1)) {
			return tag.values()[first] == second;
		} else {
			return tag.pos_id() == second;
		}
	}

} /* end ns PlTagger */
