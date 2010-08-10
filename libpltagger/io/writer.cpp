#include <libpltagger/io/writer.h>
#include <libpltagger/io/init.h>
#include <libtoki/util/foreach.h>

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

	void TokenWriter::write_token_dispose(Token* t)
	{
		write_token(*t);
		delete t;
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
