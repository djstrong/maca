#include <libpltagger/io/writer.h>
#include <libpltagger/io/init.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	TokenWriter::TokenWriter(std::ostream& os, const Tagset& tagset)
		: os_(os), tagset_(tagset), needs_footer_(true), indent_(0)
	{
	}

	TokenWriter::~TokenWriter()
	{
	}

	void TokenWriter::finish()
	{
		if (needs_footer_) {
			do_footer();
			needs_footer_ = false;
		}
	}

	std::ostream& TokenWriter::osi()
	{
		for (int i = 0; i < indent_; ++i) {
			os_ << " ";
		}
		return os_;
	}

	void TokenWriter::indent_more(int n)
	{
		indent_ += n;
	}

	void TokenWriter::indent_less(int n)
	{
		indent_ -= n;
	}

	void TokenWriter::write_token(const Token& t)
	{
		do_token(t);
	}

	void TokenWriter::write_token_dispose(Token* t)
	{
		write_token(*t);
		delete t;
	}

	void TokenWriter::write_sentence(const std::vector<Token *> &s)
	{
		do_sentence(s);
	}

	void TokenWriter::write_paragraph(const std::vector< std::vector<Token *> > &p)
	{
		do_paragraph(p);
	}

	void TokenWriter::do_sentence(const std::vector<Token *> &s)
	{
		foreach (const Token* tok, s) {
			write_token(*tok);
		}
	}

	void TokenWriter::do_paragraph(const std::vector< std::vector<Token *> > &p)
	{
		foreach (const std::vector<Token *> s, p) {
			write_sentence(s);
		}
	}

	TokenWriter* TokenWriter::create(const std::string class_id,
			std::ostream& os,
			const Tagset& tagset)
	{
		return TokenWriterFactory::Instance().CreateObject(class_id, os, tagset);
	}

	std::vector<std::string> TokenWriter::available_writer_types()
	{
		return TokenWriterFactory::Instance().RegisteredIds();
	}

	static bool registered = init_token_writers();

} /* end ns PlTagger */
