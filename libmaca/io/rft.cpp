#include <libmaca/io/rft.h>

#include <libtoki/util/foreach.h>

#include <boost/algorithm/string.hpp>

namespace Maca {

	bool RftWriter::registered = TokenWriter::register_writer<RftWriter>(
			"rft", "");


	RftWriter::RftWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params)
		: TokenWriter(os, tagset, params), warn_on_no_lexemes_(true)
	{
		foreach (const string_range& param, params) {
			std::string p = boost::copy_range<std::string>(param);
			if (p == "nowarn") {
				warn_on_no_lexemes_ = false;
			}
		}
	}

	void RftWriter::write_token(const Token& t)
	{
		os() << t.orth_utf8() << "\t";
		if (t.lexemes().empty()) {
			if (warn_on_no_lexemes_) {
				std::cerr << "No lexemes for token!";
			}
		} else {
			const Lexeme& pref = t.get_preferred_lexeme(tagset());
			std::string tag_str = tagset().tag_to_no_opt_string(pref.tag());
			os() << boost::algorithm::replace_all_copy(tag_str, ":", ".");
		}
		os() << "\n";
	}

	void RftWriter::write_sentence(const Sentence& s)
	{
		foreach (const Token* t, s.tokens()) {
			write_token(*t);
		}
		os() << "\n";
	}

	void RftWriter::write_chunk(const Chunk& c)
	{
		foreach (Sentence* s, c.sentences()) {
			write_sentence(*s);
		}
	}

} /* end ns Maca */
