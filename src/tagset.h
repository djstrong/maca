#ifndef LIBPLTAGGER_TAGSET_H
#define LIBPLTAGGER_TAGSET_H

#include "tag.h"

#include <set>
#include <map>
#include <vector>

#include <boost/range.hpp>

#include "exception.h"
#include "symboldictionary.h"

namespace PlTagger {

	class TagsetParseError : public PlTaggerError
	{
	public:
		TagsetParseError(const std::string& w, int line, const std::string& data)
			: PlTaggerError("Tagset parse error: " + w), line(line), data(data)
		{
		}

		~TagsetParseError() throw()
		{
		}

		virtual std::string info() const;

		int line;
		std::string data;
	};

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

	class Tagset
	{
	public:
		Tagset();

		void load_from_stream(std::istream& is);

		void save_to_stream(std::ostream& os);


		typedef boost::iterator_range<std::string::const_iterator> tstr_range;

		typedef std::vector< boost::iterator_range<std::string::const_iterator> > tstring_ranges;

		Tag parse_tag(const std::string& s, bool allow_extra) const;

		Tag parse_tag(const tstring_ranges& ts, bool allow_extra) const;


		std::string tag_to_string(const Tag& tag) const;


		attribute_idx_t get_value_attribute(value_idx_t id) const;

		const std::vector<value_idx_t>& get_attribute_values(attribute_idx_t a) const;

		const std::vector<attribute_idx_t>& get_pos_attributes(pos_idx_t pos) const;

		const std::vector<bool>& get_pos_attributes_mask(pos_idx_t pos) const;

	private:
		std::string id_string_;

		tagset_idx_t id_;

		SymbolDictionary<pos_idx_t> pos_dict_;
		SymbolDictionary<attribute_idx_t> attribute_dict_;
		SymbolDictionary<value_idx_t> value_dict_;

		std::vector< std::vector<value_idx_t> > attribute_values_;

		std::vector<attribute_idx_t> value_attribute_;

		std::vector< std::vector<attribute_idx_t> > pos_attributes_;

		std::vector< std::vector<bool> > pos_attributes_mask_;

	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSET_H
