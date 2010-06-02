#include "tagset.h"

#include <libtoki/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/strong_typedef.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>

namespace PlTagger {

	Tagset::Tagset()
	{
	}

	void Tagset::load_from_stream(std::istream &is)
	{
		std::string s;
		std::string sep(" \t=");
		std::set<std::string> values;
		std::set<std::string> symbols;
		typedef std::map< std::string, std::deque<std::string> > vmap_t;
		vmap_t vmap;
		typedef std::map< std::string, std::vector<attribute_idx_t> > pmap_t;
		pmap_t pmap;

		std::deque<std::string> v;
		while (is.good() && s != "[ATTR]") {
			std::getline(is, s);
			boost::algorithm::trim(s);
		}
		std::getline(is, s);
		boost::algorithm::trim(s);

		//attribute-value defs
		while (is.good() && s != "[POS]") {
			if (!s.empty() && s[0] != '#') {
				boost::algorithm::split(v, s, boost::is_any_of(sep));
				assert(v.size() > 1);
				assert(symbols.insert(v[0]).second);
				std::deque<std::string>& avalues = vmap[v[0]];
				v.pop_front();
				avalues = v;
				foreach (const std::string& s, v) {
					assert(symbols.insert(s).second);
					values.insert(s);
				}
			}
			std::getline(is, s);
			boost::algorithm::trim(s);
		}

		std::vector<std::string> vec;
		std::copy(values.begin(), values.end(), vec.begin());
		value_dict_.load_sorted_data(vec);

		vec.clear();
		foreach (const vmap_t::value_type v, vmap) {
			vec.push_back(v.first);
			attribute_values_.resize(attribute_values_.size() + 1);
			foreach (const std::string& s, v.second) {
				attribute_values_.back().push_back(value_dict_.get_id(s));
			}
		}
		attribute_dict_.load_sorted_data(vec);

		while (is.good()) {
			if (!s.empty() && s[0] != '#') {
				boost::algorithm::split(v, s, boost::is_any_of(sep));
				assert(v.size() > 1);
				assert(symbols.insert(v[0]).second);
				std::vector<attribute_idx_t>& pattrs = pmap[v[0]];
				v.pop_front();
				foreach (const std::string& s, v) {
					attribute_idx_t a = attribute_dict_.get_id(s);
					assert(attribute_dict_.is_id_valid(a));
					pattrs.push_back(a);
				}
			}
			std::getline(is, s);
			boost::algorithm::trim(s);
		}

		vec.clear();
		foreach (const pmap_t::value_type v, pmap) {
			vec.push_back(v.first);
			pos_attributes_.push_back(v.second);
			pos_attributes_mask_.push_back(std::vector<bool>(attribute_values_.size(), false));
			foreach (attribute_idx_t a, v.second) {
				pos_attributes_mask_.back()[a] = true;
			}
		}
		pos_dict_.load_sorted_data(vec);
	}

	void Tagset::save_to_stream(std::ostream &os)
	{
		os << "#Autogenerated by PlTagger\n\n";
		os << "[ATTR]\n";
		attribute_idx_t a(0);
		while (attribute_dict_.is_id_valid(a)) {
			os << attribute_dict_.get_string(a) << "\t";
			foreach (value_idx_t v, get_attribute_values(a)) {
				os << value_dict_.get_string(v) << " ";
			}
			os << "\n";
			++a;
		}
		os << "\n[POS]\n";
		pos_idx_t p(0);
		while (pos_dict_.is_id_valid(p)) {
			os << pos_dict_.get_string(p) << "\t";
			foreach (attribute_idx_t a, get_pos_attributes(p)) {
				os << attribute_dict_.get_string(a) << " ";
			}
			os << "\n";
			++p;
		}
		os << "#End autogenerated file\n";
	}

	Tag Tagset::parse_tag(const std::string &s, bool allow_extra) const
	{
		tstring_ranges fields;
		boost::algorithm::split(fields, s, boost::is_any_of(std::string(":")));
		return parse_tag(fields, allow_extra);
	}

	Tag Tagset::parse_tag(const tstring_ranges &ts, bool allow_extra) const
	{
		if (ts.empty()) throw TagParseError("Empty POS+attribute list");
		//const std::string& pos_str = boost::copy_range<std::string>(ts[0]);

		pos_idx_t pos_id = pos_dict_.get_id(ts[0]);

		if (!pos_dict_.is_id_valid(pos_id)) {
			throw TagParseError("Invalid POS");
		}

		const std::vector<bool>& valid_attrs_mask = get_pos_attributes_mask(pos_id);

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

	const std::vector<bool>& Tagset::get_pos_attributes_mask(pos_idx_t pos) const
	{
		assert(pos_dict_.is_id_valid(pos));
		return pos_attributes_mask_[pos];
	}

} /* end ns PlTagger */