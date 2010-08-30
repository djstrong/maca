#include <libmaca/morph/morfeuszanalyser.h>
#include <libmaca/token.h>
#include <libmaca/morph/morfeuszcompat.h>
#include <libmaca/util/settings.h>
#include <libtoki/util/foreach.h>

#include <morfeusz.h>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <memory>

namespace Maca {

	const char* MorfeuszAnalyser::identifier = "morfeusz";

	bool MorfeuszAnalyser::registered = MorphAnalyser::register_analyser<MorfeuszAnalyser>();

	MorfeuszError::MorfeuszError(const std::string& error, const std::string input, InterpMorf* interp)
		: MacaError("Morfeusz error: " + error), error(error), input(input), interp(interp)
	{
	}

	MorfeuszError::~MorfeuszError() throw()
	{
	}

	std::string MorfeuszError::info() const
	{
		std::stringstream ss;
		ss << what();
		if (!input.empty()) {
			ss << " for input '" << input << "'";
		}
		return ss.str();
	}

	MorfeuszAnalyser::MorfeuszAnalyser(const Tagset* tagset, Conversion::TagsetConverter* conv)
		: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(true)
	{
		if (conv_->tagset_to().id() != tagset->id()) throw TagsetMismatch("Morfeusz analyser creation", *tagset, conv->tagset_to());
		morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
	}

	MorfeuszAnalyser::MorfeuszAnalyser(const Config::Node& cfg)
		: MorphAnalyser(cfg), conv_(NULL), ign_tag_(), warn_on_ign_(false)
	{
		std::string fn = cfg.get("converter", "");
		std::ifstream ifs;
		Path::Instance().open_stream_or_throw(fn, ifs, "converter");

		Config::Node conv_cfg = Config::from_stream(ifs);
		std::auto_ptr<Conversion::TagsetConverter> c(new Conversion::TagsetConverter(conv_cfg));

		if (c->tagset_to().id() != tagset().id()) throw TagsetMismatch("Morfeusz analyser creation", tagset(), c->tagset_to());
		conv_ = c.release();

		std::string ign_tag_string = cfg.get("ign_tag", "ign");
		ign_tag_ = conv_->tagset_from().parse_simple_tag(ign_tag_string, false);
		warn_on_ign_ = cfg.get("warn_on_ign", false);
		warn_on_fold_failure_ =  cfg.get("warn_on_fold_failure", true);

		morfeusz_set_option(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
	}

	MorfeuszAnalyser::~MorfeuszAnalyser()
	{
		delete conv_;
	}

	bool MorfeuszAnalyser::process_functional(const Toki::Token &t, boost::function<void(Token *)>sink)
	{
		std::string s = t.orth_utf8();
		InterpMorf *pmorf = morfeusz_analyse(const_cast<char*>(s.c_str()));
		if (pmorf[0].p == -1) { // no analyses
			return false;
		} else if (pmorf[1].p == -1) { // only one analysis
			if (pmorf->interp) {
				std::vector<Token*> vec;
				vec.push_back(make_token(t, pmorf));
				flush_convert(vec, sink);
			} else {
				return false;
			}
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

			std::vector<Token*> unambiguous;

			int i = 0;
			while (i < node_count) {
				if (succ[i].size() > 1) { // complex case, many interps or segmentation ambiguity
					if (!unambiguous.empty()) {
						flush_convert(unambiguous, sink);
						unambiguous.clear();
					}
					int merge_node = -1;
					std::vector< std::vector< int > > paths;

					// follow all paths to the merge point
					for (size_t n = 0; n < succ[i].size(); ++n) {
						paths.push_back(std::vector<int>());
						int tse = succ[i][n];
						paths.back().push_back(tse);
						int v = pmorf[tse].k;
						while (prec[v].size() == 1) {
							if (succ[v].size() != 1) {
								throw MorfeuszError("path splits twice", s, pmorf);
							}
							tse = succ[v][0];
							paths.back().push_back(tse);
							v = pmorf[tse].k;
						}
						//assume this is the merge node, check for consistency
						if (merge_node != -1 && merge_node != v) {
							throw MorfeuszError("path merge node ambiguity", s, pmorf);
						}
						merge_node = v;
					}

					// the possible paths as token vectors
					std::vector< std::vector<Token*> > token_paths;

					// fill token paths, fold simple lemma ambiguities
					typedef std::map< std::pair<int,int>, Token*> alt_map_t;
					alt_map_t alt_map;
					for (size_t pi = 0; pi < paths.size(); ++pi) {
						bool new_path_added = false;
						foreach (int ei, paths[pi]) {
							InterpMorf* im = pmorf + ei;
							std::pair<int, int> idx = std::make_pair(im->p, im->k);
							alt_map_t::iterator it = alt_map.find(idx);
							if (it == alt_map.end()) {
								if (!new_path_added) {
									token_paths.push_back(std::vector<Token*>());
									new_path_added = true;
								}
								token_paths.back().push_back(make_token(t, pmorf + ei));
								alt_map.insert(std::make_pair(idx, token_paths.back().back()));
							} else {
								pmorf_into_token(it->second, im);
							}
						}
					}

					flush_convert(token_paths, sink);
					i = merge_node;
				} else if (!succ[i].empty()) { //simple case, only one interp
					int edge = succ[i][0];
					unambiguous.push_back(make_token(t, pmorf + edge));
					if (pmorf[edge].k != i + 1) {
						throw MorfeuszError("simple path has non-consecutive nodes", s, pmorf);
					}
					++i;
				} else { //only the last node should have no succesors
					if (i != node_count - 1) {
						throw MorfeuszError("node without succesors that is not the last node", s, pmorf);
					}
					++i;
				}
			}
			if (!unambiguous.empty()) {
				flush_convert(unambiguous, sink);
			}
		}
		return true;
	}

	Token* MorfeuszAnalyser::make_token(const Toki::Token& t, InterpMorf *im) const
	{
		Token* tt = new Token();
		if (im->p == 0) {
			tt->set_wa(t.preceeding_whitespace());
		} else {
			tt->set_wa(Toki::Whitespace::None);
		}
		pmorf_into_token(tt, im);
		return tt;
	}

	void MorfeuszAnalyser::pmorf_into_token(Token *tt, InterpMorf *im) const
	{
		tt->set_orth(UnicodeString::fromUTF8(im->forma));
		if (im->interp) {
			conv_->tagset_from().lexemes_into_token(*tt,
					UnicodeString::fromUTF8(im->haslo),
					std::make_pair(im->interp, im->interp + strlen(im->interp)));
		} else {
			if (warn_on_ign_) {
				std::cerr << "Morfeusz: tagging as ign: " << im->forma << "\n";
			}
			Lexeme ign_lex(tt->orth(), ign_tag_);
			tt->add_lexeme(ign_lex);
		}
	}

	void MorfeuszAnalyser::flush_convert(std::vector<Token *> &vec, boost::function<void(Token *)>sink)
	{
		conv_->convert_simple(vec, sink);
	}

	void MorfeuszAnalyser::flush_convert(std::vector<std::vector<Token *> > &vec, boost::function<void(Token *)>sink)
	{
		conv_->convert_ambiguous(vec, sink, warn_on_fold_failure_);
	}

} /* end ns Maca */
