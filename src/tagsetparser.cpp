#include "tagsetparser.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <libtoki/foreach.h>

namespace PlTagger {

	std::string TagsetParseError::info() const
	{
		std::stringstream ss;
		ss << "Line " << line << ": " << what() << " `" << data << "`";
		return ss.str();
	}

	Tagset TagsetParser::load_ini(const std::string &filename)
	{
		std::ifstream ifs(filename.c_str());
		return load_ini(ifs);
	}

	void TagsetParser::save_ini(const Tagset &tagset, const std::string &filename)
	{
		std::ofstream ofs(filename.c_str());
		save_ini(tagset, ofs);
	}

	Tagset TagsetParser::load_ini(std::istream &is)
	{
		int line_no = 0;
		std::string line;
		std::string sep(" \t=");
		std::set<std::string> values;
		values.insert("@null");
		std::set<std::string> symbols(values);
		typedef std::map< std::string, std::deque<std::string> > vmap_t;
		vmap_t vmap;
		typedef std::map< std::string, std::vector<attribute_idx_t> > pmap_t;
		pmap_t pmap;
		typedef std::map< std::string, std::vector<bool> > reqmap_t;
		reqmap_t reqmap;
		Tagset tagset;

		while (std::getline(is, line)) {
			boost::algorithm::trim(line);
			++line_no;
			if (line == "[ATTR]") break;
		}

		//attribute-value defs
		while (std::getline(is, line)) {
			boost::algorithm::trim(line);
			++line_no;
			if (line == "[POS]") break;
			if (!line.empty() && line[0] != '#') {
				std::deque<std::string> v;
				boost::algorithm::split(v, line, boost::is_any_of(sep),
						boost::algorithm::token_compress_on);
				if (v.size() < 3) {
					throw TagsetParseError("Attribute with less than 2 values", line_no, line);
				}
				if (!symbols.insert(v[0]).second) {
					throw TagsetParseError("Duplicate symbol", line_no, v[0]);
				}
				std::deque<std::string>& avalues = vmap[v[0]];
				v.pop_front();
				avalues = v;
				foreach (const std::string& s, v) {
					if (!symbols.insert(s).second) {
						throw TagsetParseError("Duplicate symbol", line_no, s);
					}
					values.insert(s);
				}
			}
		}

		std::vector<std::string> vec;
		std::copy(values.begin(), values.end(), std::inserter(vec, vec.begin()));
		if (vec[0] != "@null") {
			throw TagsetParseError("First value is not '@null'", line_no, vec[0]);
		}
		tagset.value_dict_.load_sorted_data(vec);

		vec.clear();
		tagset.value_attribute_.resize(values.size());
		foreach (const vmap_t::value_type v, vmap) {
			vec.push_back(v.first);
			tagset.attribute_values_.resize(tagset.attribute_values_.size() + 1);
			foreach (const std::string& s, v.second) {
				tagset.attribute_values_.back().push_back(tagset.value_dict_.get_id(s));
				tagset.value_attribute_[tagset.value_dict_.get_id(s)] = vec.size() - 1;
			}
		}
		tagset.attribute_dict_.load_sorted_data(vec);

		while (std::getline(is, line)) {
			boost::algorithm::trim(line);
			++line_no;
			if (!line.empty() && line[0] != '#') {
				std::deque<std::string> v;
				boost::algorithm::split(v, line, boost::is_any_of(sep),
						boost::algorithm::token_compress_on);
				if (!symbols.insert(v[0]).second) {
					throw TagsetParseError("Duplicate symbol", line_no, v[0]);
				}
				std::vector<attribute_idx_t>& pattrs = pmap[v[0]];
				std::vector<bool>& req_mask = reqmap[v[0]];
				req_mask.resize(tagset.attribute_dict_.size());
				v.pop_front();
				foreach (std::string s, v) {
					if (s.empty()) continue;
					bool required = true;
					if (s[0] == '[' && s[s.size() - 1] == ']') {
						required = false;
						s = s.substr(1, s.size() - 2);
					}
					attribute_idx_t a = tagset.attribute_dict_.get_id(s);
					if (!tagset.attribute_dict_.is_id_valid(a)) {
						throw TagsetParseError("Attribute name invalid", line_no, s);
					}
					pattrs.push_back(a);
					req_mask[a] = required;
				}
			}

		}

		vec.clear();
		foreach (const pmap_t::value_type v, pmap) {
			vec.push_back(v.first);
			tagset.pos_attributes_.push_back(v.second);
			tagset.pos_valid_attributes_.push_back(std::vector<bool>(tagset.attribute_values_.size(), false));
			foreach (attribute_idx_t a, v.second) {
				tagset.pos_valid_attributes_.back()[a] = true;
			}
			tagset.pos_required_attributes_.push_back(reqmap[v.first]);
		}
		tagset.pos_dict_.load_sorted_data(vec);
		if (tagset.pos_dict_.size() == 0) {
			throw TagsetParseError("No POS in tagset", 0, "");
		}
		return tagset;
	}

	void TagsetParser::save_ini(const Tagset &tagset, std::ostream &os)
	{
		os << "# Autogenerated by PlTagger\n\n";
		os << "[ATTR]\n";
		attribute_idx_t a(0);
		while (tagset.attribute_dict_.is_id_valid(a)) {
			os << tagset.attribute_dict_.get_string(a) << "\t= ";
			foreach (value_idx_t v, tagset.get_attribute_values(a)) {
				os << tagset.value_dict_.get_string(v) << " ";
			}
			os << "\n";
			++a;
		}
		os << "\n[POS]\n";
		pos_idx_t p(0);
		while (tagset.pos_dict_.is_id_valid(p)) {
			os << tagset.pos_dict_.get_string(p) << "\t= ";
			foreach (attribute_idx_t a, tagset.get_pos_attributes(p)) {
				if (tagset.pos_required_attributes_[p][a]) {
					os << tagset.attribute_dict_.get_string(a) << " ";
				} else {
					os << '[' << tagset.attribute_dict_.get_string(a) << "] ";
				}
			}
			os << "\n";
			++p;
		}
		os << "# End autogenerated file\n";
	}
} /* end ns PlTagger */