#include <libpltagger/io/reader.h>

namespace PlTagger {

	TokenReader::TokenReader(std::istream& is, const Tagset& tagset)
		: is_(is), tagset_(tagset)
	{
	}

	TokenReader::~TokenReader()
	{
	}

	Token* TokenReader::read_token()
	{
		return NULL;
	}

	std::vector<Token*> TokenReader::read_sentence()
	{
		return std::vector<Token*>();
	}

	std::vector< std::vector<Token*> > TokenReader::read_paragraph()
	{
		return std::vector< std::vector<Token*> >();
	}
} /* end ns PlTagger */
