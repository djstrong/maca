#ifndef LIBPLTAGGER_IO_XCES_H
#define LIBPLTAGGER_IO_XCES_H

#include <libpltagger/io/reader.h>
#include <libpltagger/io/writer.h>
#include <boost/scoped_ptr.hpp>
#include <libtoki/tokensource.h>
#include <deque>

namespace xmlpp {
	class Node;
}
namespace PlTagger {

	class XcesNoMorphReaderImpl;

	class XcesNoMorphReader// : public Toki::TokenSource
	{
	public:
		XcesNoMorphReader();

		~XcesNoMorphReader();

		void parse_file(const std::string& filename);

		void parse_stream(std::istream& is);

	protected:
		boost::scoped_ptr<XcesNoMorphReaderImpl> impl_;
	};



	class XcesWriter : public TokenWriter {
	public:
		XcesWriter(std::ostream& os, const Tagset& tagset, bool force_chunk=true);

		void use_indent(bool v) {
			use_indent_ = v;
		}

		void force_chunk(bool v) {
			force_chunk_ = v;
		}

		static XcesWriter* create_flat(std::ostream& os, const Tagset& tagset);
		static XcesWriter* create_nochunk(std::ostream& os, const Tagset& tagset);
		static XcesWriter* create_flat_nochunk(std::ostream& os, const Tagset& tagset);

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

		bool force_chunk_;
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
