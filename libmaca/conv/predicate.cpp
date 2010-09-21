#include <libmaca/conv/predicate.h>
#include <libpwrutils/foreach.h>

namespace Maca {
namespace Conversion {

TagPredicate::TagPredicate(const std::string& name, const Tagset& tagset)
{
	value_idx_t value = tagset.value_dictionary().get_id(name);
	if (tagset.value_dictionary().is_id_valid(value)) {
		first = tagset.get_value_attribute(value);
		second = value;
	} else {
		attribute_idx_t attr = tagset.attribute_dictionary().get_id(name);
		if (tagset.attribute_dictionary().is_id_valid(attr)) {
			first = attr;
			second = 0;
		} else {
			pos_idx_t pos = tagset.pos_dictionary().get_id(name);
			if (!tagset.pos_dictionary().is_id_valid(pos)) {
				throw MacaError("Predicate string invalid: '" + name +
						"' in tagset " + tagset.name());
			} else {
				first = static_cast<idx_t>(-1);
				second = pos;
			}
		}
	}
}

bool TagPredicate::check(const Tag &tag) const
{
	if (first != static_cast<idx_t>(-1)) {
		return tag.values()[first] == second;
	} else {
		return tag.pos_id() == second;
	}
}

bool TagPredicate::token_match(const Token& t) const
{
	if (first != static_cast<idx_t>(-1)) {
		foreach (const Lexeme& lex, t.lexemes()) {
			if (lex.tag().values()[first] != second) return false;
		}
	} else {
		foreach (const Lexeme& lex, t.lexemes()) {
			if (lex.tag().pos_id() != second) return false;
		}
	}
	return true;
}

void TagPredicate::apply(Tag &tag) const
{
	if (first != static_cast<idx_t>(-1)) {
		tag.values()[first] = static_cast<value_idx_t>(second);
	} else {
		tag.set_pos_id(static_cast<pos_idx_t>(second));
	}
}

void apply_predicates(const std::vector<TagPredicate>& v, Token& t)
{
	foreach (Lexeme& lex, t.lexemes()) {
		Tag newtag = lex.tag();
		foreach (const TagPredicate& tp, v) {
			tp.apply(newtag);
		}
		lex.set_tag(newtag);
	}
}

PosOrthPredicate::PosOrthPredicate()
{
}

PosOrthPredicate::PosOrthPredicate(pos_idx_t pos, const UnicodeString &orth)
	: std::pair<pos_idx_t, UnicodeString>(pos, orth)
{
}

bool PosOrthPredicate::check(const Token &token) const
{
	return token.orth_pos_match(first, second);
}

} /* end ns Conversion */
} /* end ns Maca */
