#include <libpltagger/tagsetconverter.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	TagsetConverter::TagsetConverter(const Tagset &from, const Tagset &to)
		: from_(from), to_(to)
	{
	}

	TagsetConverter::~TagsetConverter()
	{
	}

	void TagsetConverter::convert_simple(const std::vector<Token *>& v, boost::function<void(Token *)>sink)
	{
		foreach (Token* t, v) {
			std::set<Lexeme> new_lex;
			foreach (Lexeme& lex, t->lexemes()) {
				Tag tag2 = lex.tag();
				attribute_idx_t g_idx = tagset_from().attribute_dictionary().get_id("gnd");
				if (tag2.values()[g_idx] > 0) {
					tag2.values()[g_idx] = tagset_from().value_dictionary().get_id("m1");
				}
				new_lex.insert(Lexeme(lex.lemma(), tag2));
			}
			t->lexemes().clear();
			std::copy(new_lex.begin(), new_lex.end(), std::back_inserter(t->lexemes()));
			sink(t);
		}
	}

	void TagsetConverter::convert_ambiguous(const std::vector<std::vector<Token *> >& v, boost::function<void(Token *)>sink)
	{
		size_t min_len_path;
		//size_t min_len =
		find_shortest(v, min_len_path);
		convert_simple(v[min_len_path], sink);
	}

	size_t TagsetConverter::find_shortest(const std::vector<std::vector<Token *> >& v, size_t& min_len_path)
	{
		size_t min_len = v[0].size();
		min_len_path = 0;
		for (size_t pi = 0; pi < v.size(); ++pi) {
			if (v[pi].size() < min_len) {
				min_len = v[pi].size();
				min_len_path = pi;
			}
		}
		return min_len;
	}

} /* end ns PlTagger */
