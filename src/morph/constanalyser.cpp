#include "constanalyser.h"

namespace PlTagger {

	ConstAnalyser::ConstAnalyser(const std::string &tag)
		: tag_(tag)
	{
	}

	ConstAnalyser::~ConstAnalyser()
	{
	}

	Token* ConstAnalyser::process(const Toki::Token &t)
	{
		Token* tt = new Token(t);
		tt->add_lexeme(Lexeme(tt->orth(), tag_));
		return tt;
	}

} /* end ns PlTagger */
