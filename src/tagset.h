#ifndef LIBPLTAGGER_TAGSET_H
#define LIBPLTAGGER_TAGSET_H

#include "tag.h"

#include <set>
#include <map>
#include <vector>

#include <boost/range.hpp>
#include <boost/function.hpp>

#include "exception.h"
#include "symboldictionary.h"
#include "typedefs.h"

namespace PlTagger {

	class TagParseError : public PlTaggerError
	{
	public:
		TagParseError(const std::string &what)
		 : PlTaggerError(what)
		{
		}

		~TagParseError() throw()
		{
		}
	};

	class TagsetParser;

	class Tagset
	{
	public:
		Tagset();

		explicit Tagset(const char*);


		void parse_tag(const std::string& s, bool allow_extra, boost::function<void (const Tag&)> sink) const {
			parse_tag(string_range(s.begin(), s.end()), allow_extra, sink);
		}

		void parse_tag(const char* s, bool allow_extra, boost::function<void (const Tag&)> sink) const {
			parse_tag(string_range(s, s + strlen(s)), allow_extra, sink);
		}

		void parse_tag(const string_range& s, bool allow_extra, boost::function<void (const Tag&)> sink) const;

		void parse_tag(const string_range_vector& ts, bool allow_extra, boost::function<void (const Tag&)> sink) const;

		std::vector<Tag> parse_tag(const std::string& s, bool allow_extra) const {
			return parse_tag(string_range(s.begin(), s.end()), allow_extra);
		}

		std::vector<Tag> parse_tag(const char* s, bool allow_extra) const {
			return parse_tag(string_range(s, s + strlen(s)), allow_extra);
		}

		std::vector<Tag> parse_tag(const string_range& s, bool allow_extra) const;

		std::vector<Tag> parse_tag(const string_range_vector& ts, bool allow_extra) const;


		Tag parse_simple_tag(const std::string& s, bool allow_extra) const {
			string_range sr(s.begin(), s.end());
			return parse_simple_tag(sr, allow_extra);
		}

		Tag parse_simple_tag(const string_range& s, bool allow_extra) const;

		Tag parse_simple_tag(const string_range_vector& ts, bool allow_extra) const;



		bool validate_tag(const Tag& t, bool allow_extra);

		std::string tag_to_string(const Tag& tag) const;


		attribute_idx_t get_value_attribute(value_idx_t id) const;

		const std::vector<value_idx_t>& get_attribute_values(attribute_idx_t a) const;

		const std::vector<attribute_idx_t>& get_pos_attributes(pos_idx_t pos) const;

		const std::vector<bool>& get_pos_valid_attributes(pos_idx_t pos) const;

		const std::vector<bool>& get_pos_required_attributes(pos_idx_t pos) const;

	private:
		friend class TagsetParser;

		std::string id_string_;

		tagset_idx_t id_;

		SymbolDictionary<pos_idx_t> pos_dict_;
		SymbolDictionary<attribute_idx_t> attribute_dict_;
		SymbolDictionary<value_idx_t> value_dict_;

		std::vector< std::vector<value_idx_t> > attribute_values_;

		std::vector<attribute_idx_t> value_attribute_;

		std::vector< std::vector<attribute_idx_t> > pos_attributes_;

		std::vector< std::vector<bool> > pos_valid_attributes_;

		std::vector< std::vector<bool> > pos_required_attributes_;

	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSET_H
