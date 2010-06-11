#include "morfeuszanalyser.h"
#include "token.h"

#include <morfeusz.h>

#include <boost/bind.hpp>

namespace PlTagger {

	MorfeuszAnalyser::MorfeuszAnalyser(const Tagset *tagset)
		: MorphAnalyser(tagset)
	{
		morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
	}

	void MorfeuszAnalyser::process_functional(const Toki::Token &t, boost::function<void(Token *)>sink)
	{
		std::string s = t.orth_utf8();
		InterpMorf *pmorf = morfeusz_analyse(const_cast<char*>(s.c_str()));
		if (pmorf[0].p == -1) { // no analyses
			return;
		} else if (pmorf[1].p == -1) { // only one analysis
			sink(make_token(t, pmorf));
		} else { // token was split
			std::vector< std::vector< int > > next;
			std::vector< std::vector< int > > prev;
			int node_count = 0;
			int edge_count = 0;
			while (pmorf[edge_count].p != -1) {
				node_count = std::max(node_count, pmorf[edge_count ].k);
				++edge_count;
			}
			std::vector< std::vector<int> > adj(node_count, std::vector<int>());
			for (int i = 0; i < edge_count; ++i) {
				next[pmorf[i].p].push_back(i);
				prev[pmorf[i].k].push_back(i);
			}

		}
	}

	Token* MorfeuszAnalyser::make_token(const Toki::Token& t, InterpMorf *im) const
	{
		Token* tt = new Token();
		UnicodeString lemma = UnicodeString::fromUTF8(im->haslo);
		tt->set_orth(UnicodeString::fromUTF8(im->forma));
		if (im->p > 0) {
			tt->set_wa(t.preceeding_whitespace());
		} else {
			tt->set_wa(Toki::Whitespace::None);
		}

		boost::function<Lexeme (const Tag&)> lex;
		lex = boost::bind(&Lexeme::create, boost::cref(lemma), _1);

		boost::function<void (const Tag&)> func;
		func = boost::bind(&std::vector<Lexeme>::push_back, &tt->lexemes(), boost::bind(lex, _1));

		tagset().parse_tag(std::string(im->interp), false, func);
		return tt;
	}

} /* end ns PlTagger */
