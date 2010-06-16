#include <libpltagger/morph/constanalyser.h>

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

	void ConstAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		Token* tt = new Token(t);
		tt->add_lexeme(Lexeme(tt->orth(), tag_));
		sink(tt);
	}

} /* end ns PlTagger */
