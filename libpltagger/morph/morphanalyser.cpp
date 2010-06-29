#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/tagsetmanager.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <libtoki/foreach.h>

namespace PlTagger {

	MorphAnalyser::MorphAnalyser(const Tagset *tagset)
		: tagset_(tagset)
	{
	}

	MorphAnalyser::MorphAnalyser(const Config::Node& cfg)
		: tagset_(&get_named_tagset(cfg.get<std::string>("tagset")))
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

		string_range_vector options;
		boost::algorithm::split(options, tag, boost::is_any_of("+|"));

		foreach (string_range& sr, options) {
			if (!sr.empty()) {
				tagset().parse_tag(sr, false, func);
			}
		}
		//tagset().parse_tag(tag, false, func);
	}

} /* end ns PlTagger */
