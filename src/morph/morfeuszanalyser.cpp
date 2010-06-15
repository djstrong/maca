#include "morfeuszanalyser.h"
#include "token.h"
#include "morfeuszcompat.h"
#include <libtoki/foreach.h>

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
			std::vector<Token*> tv;
			MorfeuszCompat::Analyse(tv, s, *this);
			foreach (Token* t, tv) {
				sink(t);
			}
			return;

			std::vector< std::vector< int > > succ;
			std::vector< std::vector< int > > prec;
			int node_count = 0;
			int edge_count = 0;
			while (pmorf[edge_count].p != -1) {
				node_count = std::max(node_count, pmorf[edge_count ].k);
				++edge_count;
			}
			std::vector< std::vector<int> > adj(node_count, std::vector<int>());
			for (int i = 0; i < edge_count; ++i) {
				succ[pmorf[i].p].push_back(i);
				prec[pmorf[i].k].push_back(i);
			}
			for (int i = 0; i < node_count; ++i) {
				if (prec[i].size() < 2 && succ[i].size() < 2) {

				}
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
		if (im->interp) {
			parse_tag_into_token(tt, lemma, im->interp);
		} else {
			tt->make_ign(tagset());
		}
		return tt;
	}

} /* end ns PlTagger */
