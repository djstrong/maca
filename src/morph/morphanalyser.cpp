#include "morphanalyser.h"

#include <boost/bind.hpp>

namespace PlTagger {

	MorphAnalyser::MorphAnalyser(const Tagset *tagset)
		: tagset_(tagset)
	{
	}

	MorphAnalyser::~MorphAnalyser()
	{
	}

	void MorphAnalyser::process(const Toki::Token &t, std::vector<Token *>& vec)
	{
		process_functional(t, boost::bind(&std::vector<Token*>::push_back, &vec, _1));
	}

	std::vector<Token*> MorphAnalyser::process(const Toki::Token &t)
	{
		std::vector<Token*> v;
		process(t, v);
		return v;
	}

	void MorphAnalyser::parse_tag_into_token(Token *tok, const UnicodeString &lemma,
			const std::string &tag) const
	{
		boost::function<Lexeme (const Tag&)> lex;
		lex = boost::bind(&Lexeme::create, boost::cref(lemma), _1);

		boost::function<void (const Tag&)> func;
		func = boost::bind(&std::vector<Lexeme>::push_back, &tok->lexemes(), boost::bind(lex, _1));

		tagset().parse_tag(tag, false, func);
	}

} /* end ns PlTagger */
