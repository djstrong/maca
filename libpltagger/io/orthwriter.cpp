#include <libpltagger/io/orthwriter.h>
#include <libtoki/util/foreach.h>

namespace PlTagger {

	OrthWriter::OrthWriter(std::ostream& os, const Tagset& tagset, const string_range_vector& params)
		: TokenWriter(os, tagset, params)
	{
	}

	OrthWriter::~OrthWriter()
	{
	}

	void OrthWriter::write_token(const Token &t)
	{
		os() << Toki::Whitespace::to_whitespace(t.wa()) << t.orth_utf8();
	}

	void OrthWriter::write_sentence(const Sentence &s)
	{
		if (!s.tokens().empty()) {
			os() << s[0]->orth_utf8();
		}
		for (size_t i = 1; i < s.tokens().size(); ++i) {
			write_token(*s[i]);
		}
		os() << "\n";
	}

	void OrthWriter::write_chunk(const Chunk &c)
	{
		foreach (const Sentence* s, c.sentences()) {
			write_sentence(*s);
			os() << "\n";
		}
	}

} /* end ns PlTagger */
