#ifndef LIBPLTAGGER_IO_XCES_H
#define LIBPLTAGGER_IO_XCES_H

#include <libpltagger/io/reader.h>
#include <libpltagger/io/writer.h>

#include <deque>

namespace xmlpp {
	class Node;
}
namespace PlTagger {

	class XcesWriter : public TokenWriter {
	public:
		XcesWriter(std::ostream& os, const Tagset& tagset);

		void use_indent(bool v) {
			use_indent_ = v;
		}

		static XcesWriter* create_flat(std::ostream& os, const Tagset& tagset);

		~XcesWriter();

		void write_sentence(const std::vector<Token*>& v);

		void write_paragraph(const std::vector< std::vector<Token*> >& v);

	protected:
		void do_header();

		void do_footer();

		void do_token(const Token& t);

		void do_sentence(const std::vector<Token*>& v);

		void paragraph_head();

		void do_paragraph(const std::vector< std::vector<Token*> >& v);

		int cid_;

		bool use_indent_;
	};

	class XcesReader : public TokenReader
	{
	public:
		XcesReader(std::istream& is, const Tagset& tagset);

		~XcesReader();

		Token* token_from_tok_node(const xmlpp::Node* n);

		std::vector< std::vector<Token*> > read_paragraph();

	protected:
		std::deque< std::vector< std::vector< Token* > > > paragraphs_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCES_H
