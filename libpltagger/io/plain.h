#ifndef LIBPLTAGGER_IO_PLAIN_H
#define LIBPLTAGGER_IO_PLAIN_H

#include <libpltagger/token.h>

namespace PlTagger {

	class PlainWriter
	{
	public:
		PlainWriter(std::ostream& os, const Tagset& tagset);

		void write_token(const Token& t);

		void write_sentence(const std::vector<Token*>& t);
	private:
		std::ostream& os_;

		const Tagset& tagset_;
	};

	class PlainReader
	{
	public:
		PlainReader(std::istream& is, const Tagset& tagset);

		Token* read_token();

	private:
		std::istream& is_;

		const Tagset& tagset_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_PLAIN_H
