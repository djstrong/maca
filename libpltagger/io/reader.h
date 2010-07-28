#ifndef LIBPLTAGGER_IO_READER_H
#define LIBPLTAGGER_IO_READER_H

#include <libpltagger/token.h>

namespace PlTagger {

	/**
	 * Base class for readers of tagger tokens
	 */
	class TokenReader
	{
	public:
		TokenReader(std::istream& is, const Tagset& tagset);

		virtual ~TokenReader();

		virtual Token* read_token();

		virtual std::vector<Token*> read_sentence();

		virtual std::vector< std::vector<Token*> > read_paragraph();

		std::istream& is() {
			return is_;
		}

		const Tagset& tagset() {
			return tagset_;
		}
	private:
		std::istream& is_;

		const Tagset& tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_READER_H
