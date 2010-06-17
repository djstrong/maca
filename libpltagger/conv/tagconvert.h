#ifndef LIBPLTAGGER_TAGCONVERT_H
#define LIBPLTAGGER_TAGCONVERT_H

#include <libpltagger/tagset.h>
#include <libpltagger/tag.h>

namespace PlTagger {

	class TagConverter
	{
	public:
		TagConverter(const Tagset& from, const Tagset& to);

		Tag cast(const Tag& from) const;

	protected:
		const Tagset& tagset_from_;
		const Tagset& tagset_to_;

		typedef std::map<value_idx_t, value_idx_t> value_map_t;
		value_map_t value_mapping_;
		typedef std::map<attribute_idx_t, attribute_idx_t> attribute_map_t;
		attribute_map_t attribute_mapping_;
		typedef std::map<pos_idx_t, pos_idx_t> pos_map_t;
		pos_map_t pos_mapping_;
	};

	class TagPredicate : public std::pair<idx_t, idx_t>
	{
	public:
		TagPredicate(const std::string& name, const Tagset& tagset);
		~TagPredicate();

		bool check(const Tag& tag) const;
		void apply(Tag& tag) const;

	};


} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGCONVERT_H
