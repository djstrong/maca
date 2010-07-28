#ifndef LIBPLTAGGER_IO_WRITER_H
#define LIBPLTAGGER_IO_WRITER_H

#include <libpltagger/token.h>

namespace PlTagger {

	/**
	 * Base class for writers of tagger tokens
	 */
	class TokenWriter
	{
	public:
		TokenWriter(std::ostream& os, const Tagset& tagset);

		virtual ~TokenWriter();

		virtual void write_token(const Token& t);

		void write_token_dispose(Token* t);

		virtual void write_sentence(const std::vector<Token*>& s);

		virtual void write_paragraph(const std::vector<std::vector<Token*> >& p);

		std::ostream& os() {
			return os_;
		}

		const Tagset& tagset() {
			return tagset_;
		}

		void finish();

	protected:
		virtual void do_header() {};

		virtual void do_footer() {};

		virtual void do_token(const Token& t) = 0;

		virtual void do_sentence(const std::vector<Token*>& s) = 0;

		virtual void do_paragraph(const std::vector<std::vector<Token*> >& p) = 0;

		void indent_more(int n = 1);

		void indent_less(int n = 1);

		std::ostream& osi();

	private:
		std::ostream& os_;

		const Tagset& tagset_;

		bool needs_footer_;

		int indent_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_WRITER_H
