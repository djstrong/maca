#include <libpltagger/conv/tagconvert.h>
#include <libpltagger/token.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

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

	TagConvertLayer::TagConvertLayer(const TagConverter &tc)
		: tc_(tc)
	{
	}

	Token* TagConvertLayer::get_next_token()
	{
		Token* t = source()->get_next_token();
		if (t != NULL) {
			foreach (Lexeme& lex, t->lexemes()) {
				lex.set_tag(tc_.cast(lex.tag()));
			}
		}
		return t;
	}
} /* end ns Conversion */ } /* end ns PlTagger */
