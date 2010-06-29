#include <libpltagger/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	std::vector<attribute_idx_t> make_attribute_list(const Tagset& tagset, const std::string& str)
	{
		std::vector<attribute_idx_t> v;
		string_range_vector srv;
		boost::algorithm::split(srv, str, boost::is_any_of(": "));
		foreach (const string_range& sr, srv) {
			if (!sr.empty()) {
				attribute_idx_t a = tagset.attribute_dictionary().get_id(sr);
				if (tagset.attribute_dictionary().is_id_valid(a)) {
					v.push_back(a);
				} else {
					throw PlTaggerError("Invalid attribute in copy_attrs: " + str);
				}
			}
		}
		return v;
	}

	void copy_attributes(const Tag& from, const std::vector<attribute_idx_t>& alist, Tag& to)
	{
		foreach (attribute_idx_t a, alist) {
			to.values()[a] = from.values()[a];
		}
	}

	void copy_attributes(const Token& from, const std::vector<attribute_idx_t>& alist, Token& to)
	{
		std::vector<Lexeme> new_lexemes;
		foreach (const Lexeme& lex1, to.lexemes()) {
			foreach (const Lexeme& lex2, from.lexemes()) {
				Lexeme lex = lex1;
				copy_attributes(lex2.tag(), alist, lex.tag());
				new_lexemes.push_back(lex);
			}
		}
		to.lexemes() = new_lexemes;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
