#include "tagset.h"
#include "lexeme.h"

#include <libtoki/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <sstream>
#include <iostream>

namespace PlTagger {

	Tagset::Tagset()
	{
	}

	void Tagset::parse_tag(const string_range &s, bool allow_extra,
			boost::function<void(const Tag &)> sink) const
	{
		string_range_vector fields;
		boost::algorithm::split(fields, s, boost::is_any_of(":"));
		parse_tag(fields, allow_extra, sink);
	}

	void Tagset::parse_tag(const string_range_vector &fields, bool allow_extra,
			boost::function<void(const Tag &)>sink) const
	{
		std::vector< string_range_vector > opts(1);
		foreach (const string_range& r, fields) {
			string_range_vector dots;
			boost::algorithm::split(dots, r, boost::is_any_of("."));
			foreach (string_range_vector& o, opts) {
				o.push_back(dots.front());
			}
			size_t opts_size = opts.size();
			for (size_t di = 1; di < dots.size(); ++di) {
				for (size_t oi = 0; oi < opts_size; ++oi) {
					opts.push_back(opts[oi]);
					opts.back().back() = dots[di];
				}
			}
		}
		foreach (string_range_vector& opt, opts) {
			sink(parse_simple_tag(opt, allow_extra));
		}
	}

	std::vector<Tag> Tagset::parse_tag(const string_range& sr, bool allow_extra) const
	{
		string_range_vector fields;
		boost::algorithm::split(fields, sr, boost::is_any_of(":"));
		return parse_tag(fields, allow_extra);
	}

	std::vector<Tag> Tagset::parse_tag(const string_range_vector &fields, bool allow_extra) const
	{
		std::vector<Tag> tags;
		parse_tag(fields, allow_extra, boost::bind(&std::vector<Tag>::push_back, boost::ref(tags), _1));
		return tags;
	}

	Tag Tagset::parse_simple_tag(const string_range &s, bool allow_extra) const
	{
		string_range_vector fields;
		boost::algorithm::split(fields, s, boost::is_any_of(std::string(":")));
		return parse_simple_tag(fields, allow_extra);
	}

	Tag Tagset::parse_simple_tag(const string_range_vector &ts, bool allow_extra) const
	{
		if (ts.empty()) throw TagParseError("Empty POS+attribute list");
		//const std::string& pos_str = boost::copy_range<std::string>(ts[0]);

		pos_idx_t pos_id = pos_dict_.get_id(ts[0]);

		if (!pos_dict_.is_id_valid(pos_id)) {
			throw TagParseError("Invalid POS");
		}

		const std::vector<bool>& valid_attrs_mask = get_pos_valid_attributes(pos_id);

		Tag tag(id_, pos_id);

		for (size_t i = 1; i < ts.size(); ++i) {
			value_idx_t val_id = value_dict_.get_id(ts[i]);
			if (!value_dict_.is_id_valid(val_id)) {
				throw TagParseError("Unknown attribute value");
			}

			attribute_idx_t attr_id = get_value_attribute(val_id);

			if (valid_attrs_mask[attr_id] || allow_extra) {
				tag.values()[attr_id] = val_id;
			}
		}

		return tag;
	}

	bool Tagset::validate_tag(const Tag &t, bool allow_extra)
	{
		if (pos_dict_.is_id_valid(t.pos_id())) return false;
		std::vector<bool> valid = get_pos_valid_attributes(t.pos_id());
		std::vector<bool> required = get_pos_required_attributes(t.pos_id());
		if (t.values().size() < attribute_dict_.size()) return false;
		if (!allow_extra && t.values().size() > attribute_dict_.size()) return false;
		for (size_t i = 0; i < t.values().size(); ++i) {
			value_idx_t v = t.values()[i];
			if (v == 0) {
				if (required[i]) return false;
			} else {
				if (!valid[i] && !allow_extra) return false;
				if (!value_dict_.is_id_valid(v)) return false;
				attribute_idx_t a = value_attribute_[v];
				if (a != static_cast<attribute_idx_t>(i)) return false;
			}
		}
		return true;
	}

	std::string Tagset::tag_to_string(const Tag &tag) const
	{
		std::ostringstream ss;
		ss << pos_dict_.get_string(tag.pos_id());
		const std::vector<attribute_idx_t>& attrs = get_pos_attributes(tag.pos_id());
		foreach (const attribute_idx_t& a, attrs) {
			ss << ":";
			ss << value_dict_.get_string(tag.values()[a]);
		}
		return ss.str();
	}


	attribute_idx_t Tagset::get_value_attribute(value_idx_t id) const
	{
		assert(value_dict_.is_id_valid(id));
		return value_attribute_[id];
	}

	const std::vector<value_idx_t>& Tagset::get_attribute_values(attribute_idx_t a) const
	{
		assert(attribute_dict_.is_id_valid(a));
		return attribute_values_[a];
	}

	const std::vector<attribute_idx_t>& Tagset::get_pos_attributes(pos_idx_t pos) const
	{
		assert(pos_dict_.is_id_valid(pos));
		return pos_attributes_[pos];
	}

	const std::vector<bool>& Tagset::get_pos_valid_attributes(pos_idx_t pos) const
	{
		assert(pos_dict_.is_id_valid(pos));
		return pos_valid_attributes_[pos];
	}

	const std::vector<bool>& Tagset::get_pos_required_attributes(pos_idx_t pos) const
	{
		assert(pos_dict_.is_id_valid(pos));
		return pos_required_attributes_[pos];
	}

} /* end ns PlTagger */
