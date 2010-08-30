#ifndef LIBMACA_TAG_H
#define LIBMACA_TAG_H

#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/operators.hpp>

namespace Maca {

	class Tagset;

	/// Typedefs for the string -> index mappings
	typedef boost::uint8_t idx_t;
	BOOST_STRONG_TYPEDEF(idx_t, pos_idx_t);
	BOOST_STRONG_TYPEDEF(idx_t, attribute_idx_t);
	BOOST_STRONG_TYPEDEF(idx_t, value_idx_t);
	BOOST_STRONG_TYPEDEF(boost::uint32_t, tagset_idx_t);

	/**
	 * This is an extended version of a POS tag, which also includes a number
	 * of attributes which may have values set.
	 *
	 * The tag is stored in a binary firmat, with POS names etc. mapped to
	 * numbers. A Tagset object is needed to create tags or print their string
	 * representation.
	 *
	 * While not strictly necessary, the tagset ID of a tag is stored within.
	 * This allows more sanity checking, esp. during tagset conversion.
	 */
	class Tag
		: boost::equality_comparable<Tag>, boost::less_than_comparable<Tag>
	{
	public:
		/// Empty tag constructor
		Tag();

		/// Tagset-and-POS (no values) constructor
		Tag(tagset_idx_t tagset_id, pos_idx_t pos);

		/// Tagset-POS-values constructor
		Tag(tagset_idx_t tagset_id, pos_idx_t pos, const std::vector<value_idx_t>& values);

		/// POS (part-of-speech) accesor
		pos_idx_t pos_id() const {
			return pos_id_;
		}

		/// POS setter
		void set_pos_id(pos_idx_t v) {
			pos_id_ = v;
		}

		/// values accesor
		const std::vector<value_idx_t>& values() const {
			return values_;
		}

		/// values accesor -- nonconst reference
		std::vector<value_idx_t>& values() {
			return values_;
		}

		/// debug aid, dump the tag's internal numeric representation
		std::string raw_dump() const;

		/// tagset id accesor
		tagset_idx_t tagset_id() const {
			return tagset_id_;
		}

		bool has_valid_tagset() const;

		/**
		 * Tag comparison. Tags sort by tagset id, then pos id, and finally
		 * value-by-value. Boost is used to provide other comparison operators.
		 */
		bool operator<(const Tag& other) const;

		/**
		 * Tag equality. Non-equality is provided by Boost template magic.
		 */
		bool operator==(const Tag& other) const;

	private:
		/// the POS id
		pos_idx_t pos_id_;
\
		/// the values
		std::vector<value_idx_t> values_;

		/// the tagset id
		tagset_idx_t tagset_id_;

		friend size_t hash_value(const Tag &tag);
	};

} /* end ns Maca */


#endif // LIBMACA_TAG_H
