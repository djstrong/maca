#ifndef LIBPLTAGGER_TAGCONVERT_H
#define LIBPLTAGGER_TAGCONVERT_H

#include <libpltagger/tagset.h>
#include <libpltagger/conv/layer.h>
#include <libpltagger/confignode.h>


namespace PlTagger { namespace Conversion {

	/**
	 * A tag-level converter that maps values from one tagset into another.
	 * The default behavior is to map by an exact name match, so that a value
	 * foo in attribute bar of pos baz will remain so in the output tagset.
	 * If some names have no counterparts in the output tagset, the value will
	 * not be set at all. Overrides are possible to correct such cases and to
	 * perform some basic rearranging.
	 */
	class TagConverter
	{
	public:
		/**
		 * Tag converter constructor, fills the action maps with name-based
		 * defaults.
		 */
		TagConverter(const Tagset& from, const Tagset& to);

		/**
		 * The actual conversion function which takes a tag from one tagset
		 * and returns a converted tag in another tagset.
		 */
		Tag cast(const Tag& from) const;

		/// The input tagset
		const Tagset& tagset_from() const {
			return tagset_from_;
		}

		/// The output tagset
		const Tagset& tagset_to() const {
			return tagset_to_;
		}

		/**
		 * Override function, make the a string in the input tagset map to
		 * a string in the output tagset.
		 */
		void add_override(const std::string& from, const std::string& to);

		/**
		 * Checks if all POS and values are properly mapped
		 * @returns true if the mapping is complete
		 */
		bool is_complete(std::ostream* os = NULL) const;

	protected:
		/// Input tagset
		const Tagset& tagset_from_;
		/// Output tagset
		const Tagset& tagset_to_;

		/// Value mapping
		typedef std::map<value_idx_t, value_idx_t> value_map_t;
		value_map_t value_mapping_;

		/// Attribute mapping
		typedef std::map<attribute_idx_t, attribute_idx_t> attribute_map_t;
		attribute_map_t attribute_mapping_;

		/// POS mapping
		typedef std::map<pos_idx_t, pos_idx_t> pos_map_t;
		pos_map_t pos_mapping_;
	};

	/**
	 * A layer wrapper around a TagConverter
	 */
	class TagConvertLayer : public Layer
	{
	public:
		/**
		 * Create a TagConvertLayer from an existing TagConverter
		 */
		TagConvertLayer(const TagConverter& tc);

		/**
		 * Create a TagConvertLayer from a config
		 */
		TagConvertLayer(const Config::Node& cfg);

		/// TokeSource override
		Token* get_next_token();

		/// Layer override -- the input tagset
		const Tagset& tagset_from() const;

		/// Layer override -- the output tagset
		const Tagset& tagset_to() const;

	private:
		/// The tag converter object
		TagConverter tc_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGCONVERT_H
