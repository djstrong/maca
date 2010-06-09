#include "constanalyser.h"

namespace PlTagger {

	ConstAnalyser::ConstAnalyser(const Tagset *tagset, const std::string &tag)
		: MorphAnalyser(tagset), tag_(tagset->parse_simple_tag(tag, false))
	{
	}

	ConstAnalyser::ConstAnalyser(const Tagset *tagset, const Tag &tag)
		: MorphAnalyser(tagset), tag_(tag)
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
