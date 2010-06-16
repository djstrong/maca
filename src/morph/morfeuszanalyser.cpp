#include "morfeuszanalyser.h"
#include "token.h"
#include "morfeuszcompat.h"
#include <libtoki/foreach.h>

#include <morfeusz.h>

#include <boost/bind.hpp>
#include <boost/graph/edge_list.hpp>

namespace PlTagger {

	MorfeuszAnalyser::MorfeuszAnalyser(const Tagset *tagset)
		: MorphAnalyser(tagset)
	{
		morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
	}

	struct morf_edge
	{
		typedef int value_type;
		typedef int first_type;
		typedef int second_type;
		int first, second;
		char* forma, haslo, interp;
	};

	void MorfeuszAnalyser::process_functional(const Toki::Token &t, boost::function<void(Token *)>sink)
	{
		std::string s = t.orth_utf8();
		InterpMorf *pmorf = morfeusz_analyse(const_cast<char*>(s.c_str()));
		if (pmorf[0].p == -1) { // no analyses
			return;
		} else if (pmorf[1].p == -1) { // only one analysis
			sink(make_token(t, pmorf));
		} else { // token was split, or there are multiple analyses (lemmas)
			int node_count = 0, edge_count = 0;
			while (pmorf[edge_count].p != -1) {
				node_count = std::max(node_count, pmorf[edge_count].k);
				++edge_count;
			}
			++node_count; //we got the last vertex id, and they start at 0, so add one
			std::vector< std::vector< int > > succ(node_count, std::vector<int>());
			std::vector< std::vector< int > > prec(node_count, std::vector<int>());
			for (int i = 0; i < edge_count; ++i) { // build adjacency lists
				succ[pmorf[i].p].push_back(i);
				prec[pmorf[i].k].push_back(i);
			}

			for (int i = 0; i < node_count; ++i) {
				if (succ[i].size() > 1) { // complex case, many interps or segmentation ambiguity
					int merge = -1;
					std::vector< std::vector< int > > paths;
					// follow all paths to the merge point
					for (size_t n = 0; n < succ[i].size(); ++n) {
						paths.push_back(std::vector<int>());
						int tse = succ[i][n];
						paths.back().push_back(tse);
						int v = pmorf[tse].k;
						while (prec[v].size() == 1) {
							assert(succ[v].size() == 1);
							tse = succ[v][0];
							paths.back().push_back(tse);
							v = pmorf[tse].k;
						}
						//assume this is the merge node
						assert(merge == -1 || merge == v);
						merge = v;
					}

					// find shortest path
					size_t min_len = paths[0].size();
					size_t min_len_path = 0;
					for (size_t pi = 0; pi < paths.size(); ++pi) {
						if (paths[pi].size() < min_len) {
							min_len = paths[pi].size();
							min_len_path = pi;
						}
					}
					if (min_len > 1) { // output shortest path only
						std::cerr << "minlen>1\n";
						foreach (int ei, paths[min_len_path]) {
							sink(make_token(t, pmorf + ei));
						}
					} else { // merge all length-1 paths as lexemes
						int e = paths[min_len_path][0];
						Token* tt = make_token(t, pmorf + e);
						for (size_t pi = 0; pi < paths.size(); ++pi) {
							if (pi == min_len_path) continue;
							if (paths[pi].size() != min_len) continue;
							pmorf_into_token(tt, pmorf + paths[pi][0]);
						}
						sink(tt);
					}
					i = merge;
				} else if (!succ[i].empty()) { //simple case, only one interp
					int edge = succ[i][0];
					sink(make_token(t, pmorf + edge));
					assert(pmorf[edge].k == i + 1);
				} else { //only the last node should have no succesors
					assert(i == node_count - 1);
				}
			}
		}
	}

	Token* MorfeuszAnalyser::make_token(const Toki::Token& t, InterpMorf *im) const
	{
		Token* tt = new Token();
		if (im->p > 0) {
			tt->set_wa(t.preceeding_whitespace());
		} else {
			tt->set_wa(Toki::Whitespace::None);
		}
		pmorf_into_token(tt, im);
		return tt;
	}

	void MorfeuszAnalyser::pmorf_into_token(Token *tt, InterpMorf *im) const
	{
		UnicodeString lemma = UnicodeString::fromUTF8(im->haslo);
		tt->set_orth(UnicodeString::fromUTF8(im->forma));
		if (im->interp) {
			parse_tag_into_token(tt, lemma, im->interp);
		} else {
			tt->add_ign(tagset());
		}
	}

} /* end ns PlTagger */
