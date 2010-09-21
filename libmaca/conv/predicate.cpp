#include <libmaca/conv/predicate.h>
#include <libpwrutils/foreach.h>
#include <libmaca/exception.h>

namespace Maca {
namespace Conversion {

TagPredicate::TagPredicate(const std::string& name, const Corpus2::Tagset& tagset)
{
	Corpus2::value_idx_t value = tagset.value_dictionary().get_id(name);
	if (tagset.value_dictionary().is_id_valid(value)) {
		first = tagset.get_value_attribute(value);
		second = value;
	} else {
		Corpus2::attribute_idx_t attr = tagset.attribute_dictionary().get_id(name);
		if (tagset.attribute_dictionary().is_id_valid(attr)) {
			first = attr;
			second = 0;
		} else {
			Corpus2::pos_idx_t pos = tagset.pos_dictionary().get_id(name);
			if (!tagset.pos_dictionary().is_id_valid(pos)) {
				throw MacaError("Predicate string invalid: '" + name +
						"' in tagset " + tagset.name());
			} else {
				first = static_cast<Corpus2::idx_t>(-1);
				second = pos;
			}
		}
	}
}

bool TagPredicate::check(const Corpus2::Tag &tag) const
{
	if (first != static_cast<Corpus2::idx_t>(-1)) {
		return tag.values()[first] == second;
	} else {
		return tag.pos_id() == second;
	}
}

bool TagPredicate::token_match(const Corpus2::Token& t) const
{
	if (first != static_cast<Corpus2::idx_t>(-1)) {
		foreach (const Corpus2::Lexeme& lex, t.lexemes()) {
			if (lex.tag().values()[first] != second) return false;
		}
	} else {
		foreach (const Corpus2::Lexeme& lex, t.lexemes()) {
			if (lex.tag().pos_id() != second) return false;
		}
	}
	return true;
}

void TagPredicate::apply(Corpus2::Tag &tag) const
{
	if (first != static_cast<Corpus2::idx_t>(-1)) {
		tag.values()[first] = static_cast<Corpus2::value_idx_t>(second);
	} else {
		tag.set_pos_id(static_cast<Corpus2::pos_idx_t>(second));
	}
}

void apply_predicates(const std::vector<TagPredicate>& v, Corpus2::Token& t)
{
	foreach (Corpus2::Lexeme& lex, t.lexemes()) {
		Corpus2::Tag newtag = lex.tag();
		foreach (const TagPredicate& tp, v) {
			tp.apply(newtag);
		}
		lex.set_tag(newtag);
	}
}

PosOrthPredicate::PosOrthPredicate()
{
}

PosOrthPredicate::PosOrthPredicate(Corpus2::pos_idx_t pos, const UnicodeString &orth)
	: std::pair<Corpus2::pos_idx_t, UnicodeString>(pos, orth)
{
}

bool PosOrthPredicate::check(const Corpus2::Token &token) const
{
	return token.orth_pos_match(first, second);
}

} /* end ns Conversion */
} /* end ns Maca */
