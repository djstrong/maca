#include <libpltagger/exception.h>
#include <libpltagger/tagset.h>
#include <libpltagger/lexeme.h>
#include <libpltagger/settings.h>
#include <libpltagger/tagsetparser.h>

#include <libtoki/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

namespace PlTagger {

	Tagset::Tagset()
		: id_(-1)
	{
	}

	Tagset::Tagset(const char *s)
		: id_(-1)
	{
		std::stringstream ss;
		ss << s;
		*this = TagsetParser::load_ini(ss);
	}

	void Tagset::parse_tag(const string_range &s, bool allow_extra,
			boost::function<void(const Tag &)> sink) const
	{
		string_range_vector fields;
		boost::algorithm::split(fields, s, boost::is_any_of(":"));
		parse_tag(fields, allow_extra, sink);
	}

	namespace {
		void append_to_multi_tag(std::vector< std::vector<value_idx_t> > & current,
				const std::vector<value_idx_t> & to_add)
		{
			foreach (std::vector<value_idx_t>& o, current) {
				o.push_back(to_add[0]);
			}
			size_t current_size = current.size();
			for (size_t ai = 1; ai < to_add.size(); ++ai) {
				for (size_t oi = 0; oi < current_size; ++oi) {
					current.push_back(current[oi]);
					current.back().back() = to_add[ai];
				}
			}
		}

	}

	void Tagset::parse_tag(const string_range_vector &fields, bool allow_extra,
			boost::function<void(const Tag &)>sink) const
	{
		if (fields.empty()) {
			throw TagParseError("No POS");
		}
		pos_idx_t pos_id = pos_dict_.get_id(fields[0]);
		if (!pos_dict_.is_id_valid(pos_id)) {
			throw TagParseError("Invalid POS: " + boost::copy_range<std::string>(fields[0]));
		}
		std::vector< std::vector<value_idx_t> > opts(1);
		for (size_t fi = 1; fi < fields.size(); ++fi) {
			const string_range& r = fields[fi];
			if (r.size() != 1 || *r.begin() != '_') {
				string_range_vector dots;
				boost::algorithm::split(dots, r, boost::is_any_of("."));
				std::vector<value_idx_t> values;
				foreach (string_range& dot, dots) {
					value_idx_t v = value_dict_.get_id(dot);
					if (!value_dict_.is_id_valid(v)) {
						throw TagParseError("Unknown attribute value: " + boost::copy_range<std::string>(r));
					}
					values.push_back(v);
				}
				append_to_multi_tag(opts, values);
			} else if (!r.empty()) { // underscore handling
				if (fi - 1 >= pos_attributes_[pos_id].size()) {
					throw TagParseError("Underscore beyond the last attribute for this POS");
				}
				attribute_idx_t attr = pos_attributes_[pos_id][fi - 1];
				append_to_multi_tag(opts, attribute_values_[attr]);
			} // else empty, do nothing
		}
		foreach (std::vector<value_idx_t>& opt, opts) {
			sink(make_tag(pos_id, opt, allow_extra));
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
		pos_idx_t pos_id = pos_dict_.get_id(ts[0]);
		if (!pos_dict_.is_id_valid(pos_id)) {
			throw TagParseError("Invalid POS: " + boost::copy_range<std::string>(ts[0]));
		}
		const std::vector<bool>& valid_attrs_mask = get_pos_valid_attributes(pos_id);

		Tag tag(id_, pos_id);
		std::vector<value_idx_t> vvv(attribute_dict_.size(), static_cast<value_idx_t>(0));
		tag.values().swap(vvv);

		for (size_t i = 1; i < ts.size(); ++i) {
			if (!ts[i].empty()) {
				value_idx_t val_id = value_dict_.get_id(ts[i]);
				if (!value_dict_.is_id_valid(val_id)) {
					throw TagParseError("Unknown attribute value: " + boost::copy_range<std::string>(ts[i]));
				}
				attribute_idx_t attr_id = get_value_attribute(val_id);
				if (valid_attrs_mask[attr_id] || allow_extra) {
					tag.values()[attr_id] = val_id;
				}
			}
		}
		return tag;
	}
	
	Tag Tagset::make_tag(pos_idx_t pos, const std::vector<value_idx_t>& values, bool allow_extra) const
	{
		const std::vector<bool>& valid_attrs_mask = get_pos_valid_attributes(pos);

		Tag tag(id_, pos);
		std::vector<value_idx_t> vvv(attribute_dict_.size(), static_cast<value_idx_t>(0));
		tag.values().swap(vvv);

		for (size_t i = 0; i < values.size(); ++i) {
			value_idx_t val_id = values[i];
			attribute_idx_t attr_id = get_value_attribute(val_id);
			if (valid_attrs_mask[attr_id] || allow_extra) {
				tag.values()[attr_id] = val_id;
			} else {
				std::stringstream ss;
				ss << "Attribute not valid for this POS: "
					<< pos_dict_.get_string(pos) << " - "
					<< attribute_dict_.get_string(attr_id);
				throw TagParseError(ss.str());
			}
		}
		return tag;
	}

	Tag Tagset::make_ign_tag() const
	{
		pos_idx_t ign_pos = pos_dictionary().get_id("ign");
		assert(pos_dictionary().is_id_valid(ign_pos));
		Tag tag(id_, ign_pos);
		tag.values().resize(attribute_dict_.size(), static_cast<value_idx_t>(0));
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
			if (pos_required_attributes_[tag.pos_id()][a] || tag.values()[a] > 0) {
				ss << ":";
				if (tag.values()[a] > 0) {
					ss << value_dict_.get_string(tag.values()[a]);
				}
			}
		}
		// print extra attributes
		for (size_t i = 0; i < attribute_dict_.size(); ++i) {
			if (tag.values()[i] > 0 && !pos_valid_attributes_[tag.pos_id()][i]) {
				ss << ":" << value_dict_.get_string(tag.values()[i]);
			}
		}
		return ss.str();
	}


	attribute_idx_t Tagset::get_value_attribute(value_idx_t id) const
	{
		if (!value_dict_.is_id_valid(id)) {
			std::stringstream ss;
			ss << "get_value_attribute fail " << (int)id;
			throw PlTaggerError(ss.str());
		}
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

	size_t Tagset::size() const
	{
		size_t sum = 0;
		for (size_t p = 0; p < pos_dict_.size(); ++p) {
			size_t pos_size = 1;
			for (size_t i = 0; i < pos_attributes_[p].size(); ++i) {
				attribute_idx_t a = pos_attributes_[p][i];
				if (pos_required_attributes_[p][a]) {
					pos_size *= attribute_values_[a].size();
				} else {
					pos_size *= (attribute_values_[a].size() + 1);
				}
			}
			sum += pos_size;
		}
		return sum;
	}

	double Tagset::size_extra() const
	{
		double vs = 1;
		for (size_t a = 0; a < attribute_dict_.size(); ++a) {
			vs *= (attribute_values_[a].size() + 1);
		}
		vs *= pos_dict_.size();
		return vs;
	}

} /* end ns PlTagger */
