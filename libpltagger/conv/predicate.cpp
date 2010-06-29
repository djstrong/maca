#include <libpltagger/conv/predicate.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TagPredicate::TagPredicate(const std::string& name, const Tagset& tagset)
	{
		second = tagset.value_dictionary().get_id(name);
		if (tagset.value_dictionary().is_id_valid(static_cast<value_idx_t>(second))) {
			first = tagset.get_value_attribute(static_cast<value_idx_t>(second));
		} else {
			first = tagset.attribute_dictionary().get_id(name);
			if (tagset.attribute_dictionary().is_id_valid(static_cast<attribute_idx_t>(first))) {
				second = 0;
			} else {
				first = static_cast<idx_t>(-1);
				second = tagset.pos_dictionary().get_id(name);

				if (!tagset.pos_dictionary().is_id_valid(static_cast<pos_idx_t>(second))) {
					throw PlTaggerError("Predicate string invalid: '" + name + "' in tagset " + tagset.name());
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

	void TagPredicate::token_apply(Token& t) const
	{
		if (first != static_cast<idx_t>(-1)) {
			foreach (Lexeme& lex, t.lexemes()) {
				lex.tag().values()[first] = second;
			}
		} else {
			foreach (Lexeme& lex, t.lexemes()) {
				lex.tag().pos_id() = second;
			}
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
} /* end ns Conversion */ } /* end ns PlTagger */
