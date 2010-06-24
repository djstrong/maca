#ifndef LIBPLTAGGER_TAGCONVERT_H
#define LIBPLTAGGER_TAGCONVERT_H

#include <libpltagger/tagset.h>
#include <libpltagger/conv/layer.h>
#include <libtoki/confignode.h>


namespace PlTagger { namespace Conversion {

	class TagConverter
	{
	public:
		TagConverter(const Tagset& from, const Tagset& to);

		Tag cast(const Tag& from) const;

		const Tagset& tagset_from() const {
			return tagset_from_;
		}

		const Tagset& tagset_to() const {
			return tagset_to_;
		}

		void add_override(const std::string& from, const std::string& to);

		/**
		 * Checks if all POS and values are properly mapped
		 * @returns true if the mapping is complete
		 */
		bool is_complete(std::ostream* os = NULL) const;

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

	class TagConvertLayer : public Layer
	{
	public:
		TagConvertLayer(const TagConverter& tc);

		TagConvertLayer(const Toki::Config::Node& cfg);

		Token* get_next_token();

		const Tagset& tagset_from() const;

		const Tagset& tagset_to() const;

	private:
		TagConverter tc_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGCONVERT_H
