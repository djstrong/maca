#ifndef LIBMACA_TAGCONVERT_H
#define LIBMACA_TAGCONVERT_H

#include <libmaca/tagset.h>
#include <libmaca/conv/layer.h>
#include <libmaca/util/confignode.h>


namespace Maca { namespace Conversion {

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
		 * @param os Optional stream for error explanation
		 * @param all Continue checking and outputting errors after a fault
		 * @returns true if the mapping is complete
		 */
		bool is_complete(std::ostream* os = NULL, bool all = false) const;

		void set_late_check(bool v) {
			late_check_ = v;
		}

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

		bool late_check_;
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

} /* end ns Conversion */ } /* end ns Maca */

#endif // LIBMACA_TAGCONVERT_H
