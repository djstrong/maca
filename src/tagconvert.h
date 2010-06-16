#ifndef LIBPLTAGGER_TAGCONVERT_H
#define LIBPLTAGGER_TAGCONVERT_H

#include "tagset.h"

namespace PlTagger {

	class TagConverter
	{
	public:
		TagConverter(const Tagset& from, const Tagset& to);

		Tag cast(const Tag& from) const;

	protected:
		const Tagset& tagset_from;
		const Tagset& tagset_to;

		typedef std::map<value_idx_t, value_idx_t> value_map_t;
		value_map_t value_mapping;
		typedef std::map<attribute_idx_t, attribute_idx_t> attribute_map_t;
		attribute_map_t attribute_mapping_;
		typedef std::map<pos_idx_t, pos_idx_t> pos_map_t;
		pos_map_t pos_mapping_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGCONVERT_H
