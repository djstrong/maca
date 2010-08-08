#ifndef LIBPLTAGGER_IO_XCESREADER_H
#define LIBPLTAGGER_IO_XCESREADER_H

#include <libpltagger/io/reader.h>
#include <deque>

namespace xmlpp {
	class Node;
}

namespace PlTagger {

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

#endif // LIBPLTAGGER_IO_XCESREADER_H
