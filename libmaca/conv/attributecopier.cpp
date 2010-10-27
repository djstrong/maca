#include <libmaca/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>
#include <libpwrutils/foreach.h>
#include <libmaca/typedefs.h>
#include <libmaca/exception.h>

namespace Maca {
namespace Conversion {

Corpus2::mask_t make_attributes_mask(const Corpus2::Tagset& tagset,
		const std::string& str)
{
	Corpus2::mask_t v = 0;
	return append_attributes_mask(v, tagset, str);
}

Corpus2::mask_t append_attributes_mask(Corpus2::mask_t& v,
		const Corpus2::Tagset& tagset, const std::string& str)
{
	string_range_vector srv;
	boost::algorithm::split(srv, str, boost::is_any_of(std::string(": ")));
	foreach (const string_range& sr, srv) {
		if (!sr.empty()) {
			Corpus2::mask_t a = tagset.get_attribute_mask(sr);
			if (a.any()) {
				v |= a;
			} else {
				throw MacaError("Invalid attribute in copy_attrs: " + str);
			}
		}
	}
	return v;
}

void copy_attributes(const Corpus2::Tag& from,
		const Corpus2::mask_t& amask, Corpus2::Tag& to)
{
	to.add_values_masked(from.get_values(), amask);
}

void copy_attributes(const Corpus2::Token& from,
		const Corpus2::mask_t& alist, Corpus2::Token& to)
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
