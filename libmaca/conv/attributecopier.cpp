#include <libmaca/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>
#include <libpwrutils/foreach.h>
#include <libmaca/typedefs.h>
#include <libmaca/exception.h>

namespace Maca {
namespace Conversion {

std::vector<Corpus2::attribute_idx_t> make_attribute_list(const Corpus2::Tagset& tagset,
		const std::string& str)
{
	std::vector<Corpus2::attribute_idx_t> v;
	append_attribute_list(v, tagset, str);
	return v;
}

void append_attribute_list(std::vector<Corpus2::attribute_idx_t>& v,
		const Corpus2::Tagset& tagset, const std::string& str)
{
	string_range_vector srv;
	boost::algorithm::split(srv, str, boost::is_any_of(std::string(": ")));
	foreach (const string_range& sr, srv) {
		if (!sr.empty()) {
			Corpus2::attribute_idx_t a = tagset.attribute_dictionary().get_id(sr);
			if (tagset.attribute_dictionary().is_id_valid(a)) {
				v.push_back(a);
			} else {
				throw MacaError("Invalid attribute in copy_attrs: " + str);
			}
		}
	}
}

void copy_attributes(const Corpus2::Tag& from,
		const std::vector<Corpus2::attribute_idx_t>& alist, Corpus2::Tag& to)
{
	foreach (Corpus2::attribute_idx_t a, alist) {
		to.values()[a] = from.values()[a];
	}
}

void copy_attributes(const Corpus2::Token& from,
		const std::vector<Corpus2::attribute_idx_t>& alist, Corpus2::Token& to)
{
	std::vector<Corpus2::Lexeme> new_lexemes;
	foreach (const Corpus2::Lexeme& lex1, to.lexemes()) {
		foreach (const Corpus2::Lexeme& lex2, from.lexemes()) {
			Corpus2::Tag tag = lex1.tag();
			copy_attributes(lex2.tag(), alist, tag);
			Corpus2::Lexeme lex3(lex1.lemma(), tag);
			if (lex1.is_disamb() && lex2.is_disamb()) {
				lex3.set_disamb(true);
			}
			new_lexemes.push_back(lex3);
		}
	}
	to.lexemes() = new_lexemes;
	to.remove_duplicate_lexemes();
}

} /* end ns Conversion */
} /* end ns Maca */
