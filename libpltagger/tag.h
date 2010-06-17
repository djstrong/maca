#ifndef LIBPLTAGGER_TAG_H
#define LIBPLTAGGER_TAG_H

#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/strong_typedef.hpp>

namespace PlTagger {

	class Tagset;

	/// Typedefs for the string -> index mappings
	typedef boost::uint8_t idx_t;
	BOOST_STRONG_TYPEDEF(idx_t, pos_idx_t);
	BOOST_STRONG_TYPEDEF(idx_t, attribute_idx_t);
	BOOST_STRONG_TYPEDEF(idx_t, value_idx_t);
	BOOST_STRONG_TYPEDEF(idx_t, tagset_idx_t);

	class Tag
	{
	public:
		Tag();

		explicit Tag(const std::string& str);

		Tag(tagset_idx_t tagset_id, pos_idx_t pos);

		Tag(tagset_idx_t tagset_id, pos_idx_t pos, const std::vector<value_idx_t>& values);

		std::string to_string() const;

		pos_idx_t pos_id() const {
			return pos_id_;
		}

		void set_pos_id(pos_idx_t v) {
			pos_id_ = v;
		}

		const std::string& pos_string() const;


		const std::vector<value_idx_t>& values() const {
			return values_;
		}

		std::vector<value_idx_t>& values() {
			return values_;
		}

		std::vector<std::string>& values_string_vector() const;


		tagset_idx_t tagset_id() const {
			return tagset_id_;
		}

		const Tagset& tagset() const;

		bool operator<(const Tag& other) const;

		bool operator==(const Tag& other) const;

	private:
		pos_idx_t pos_id_;

		std::vector<value_idx_t> values_;

		tagset_idx_t tagset_id_;

		std::string str_;
	};

} /* end ns PlTagger */


#endif // LIBPLTAGGER_TAG_H
