#include <libmaca/io/orthwriter.h>
#include <libtoki/util/foreach.h>

namespace Maca {

	OrthWriter::OrthWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& params)
		: TokenWriter(os, tagset, params), actual_ws_(false), end_nl_(false)
	{
		foreach (const string_range& param, params) {
			std::string p = boost::copy_range<std::string>(param);
			if (p == "actual_ws") {
				actual_ws_ = true;
			} else if (p == "end_nl") {
				end_nl_ = true;
			}
		}
	}

	OrthWriter::~OrthWriter()
	{
		if (end_nl_) {
			os() << "\n";
		}
	}

	void OrthWriter::write_token(const Token &t)
	{
		os() << Toki::Whitespace::to_whitespace(t.wa()) << t.orth_utf8();
	}

	void OrthWriter::write_sentence(const Sentence &s)
	{
		if (!s.tokens().empty()) {
			if (actual_ws_) {
				write_token(*s[0]);
			} else {
				os() << s[0]->orth_utf8();
			}
		}
		for (size_t i = 1; i < s.tokens().size(); ++i) {
			write_token(*s[i]);
		}
		if (!actual_ws_) {
			os() << "\n";
		}
	}

	void OrthWriter::write_chunk(const Chunk &c)
	{
		foreach (const Sentence* s, c.sentences()) {
			write_sentence(*s);
			if (!actual_ws_) {
				os() << "\n";
			}
		}
	}

} /* end ns Maca */
