#include <libmaca/morph/morfeuszanalyser.h>
#include <libmaca/token.h>
#include <libmaca/morph/morfeuszcompat.h>
#include <libmaca/util/settings.h>
#include <libtoki/util/foreach.h>
#include <libtoki/util/util.h>

#include <morfeusz.h>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/mutex.hpp>
#include <memory>
#include <dlfcn.h>

namespace Maca {

	const char* MorfeuszAnalyser::identifier = "morfeusz";

	bool MorfeuszAnalyser::registered =
			MorphAnalyser::register_analyser<MorfeuszAnalyser>();

	namespace {
		boost::mutex morfeusz_mutex_;
	}

	MorfeuszEdge::MorfeuszEdge(const InterpMorf& m)
		: node_from(m.p), node_to(m.k)
		, orth(UnicodeString::fromUTF8(m.forma))
		, lemma(m.haslo ? UnicodeString::fromUTF8(m.haslo) : UnicodeString())
		, tag_string(m.interp ? m.interp : std::string()), token(NULL)
	{
	}

	std::vector<MorfeuszEdge> morfeusz_preprocess(InterpMorf* pmorf)
	{
		std::vector<MorfeuszEdge> v;
		int i = 0;
		while (pmorf[i].p != -1) {
			v.push_back(MorfeuszEdge(pmorf[i]));
			++i;
		}
		return v;
	}

	std::vector<MorfeuszEdge> morfeusz_safe_analyse(const UnicodeString& orth)
	{
		std::string s = Toki::Util::to_utf8(orth);
		//boost::mutex::scoped_lock lock(morfeusz_mutex_);
		//InterpMorf *pmorf = morfeusz_analyse(const_cast<char*>(s.c_str()));
		return morfeusz_preprocess(0);
	}

	MorfeuszInitError::MorfeuszInitError(const std::string& error,
			const std::string& extra, const std::string& name)
		: MacaError("Morfeusz init error: " + error)
		, error(error), extra(extra), name(name)
	{
	}

	MorfeuszInitError::~MorfeuszInitError() throw()
	{
	}

	std::string MorfeuszInitError::info() const
	{
		std::stringstream ss;
		ss << what();
		ss << " (" << name << ")";
		if (!extra.empty()) {
			ss << ":\n" << extra << "\n";
		}
		return ss.str();
	}

	MorfeuszError::MorfeuszError(const std::string& error,
			const std::string input,
			const std::vector<MorfeuszEdge>& interp)
		: MacaError("Morfeusz error: " + error), error(error), input(input)
		, interp(interp)
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

	MorfeuszAnalyser::MorfeuszAnalyser(const Tagset* tagset,
			Conversion::TagsetConverter* conv, const std::string& libname,
			const std::string& require_version)
		: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(true)
		, morfeusz_library_(libname), require_version_(require_version)
	{
		if (conv_->tagset_to().id() != tagset->id()) {
			throw TagsetMismatch("Morfeusz analyser creation", *tagset,
					conv->tagset_to());
		}
		load_morfeusz_library();
	}

	void MorfeuszAnalyser::load_morfeusz_library()
	{
		morfeusz_lib_handle_ = dlmopen(LM_ID_NEWLM, morfeusz_library_.c_str(),
				RTLD_NOW);
		if (morfeusz_lib_handle_ == NULL) {
			const char* dle = dlerror();
			if (dle != NULL) {
				throw MorfeuszInitError("Error opening library (no handle)",
						dle, morfeusz_library_);
			}
		}

		dlerror();
		morfeusz_analyse_handle_ = reinterpret_cast<morfeusz_func_t>(
				dlsym(morfeusz_lib_handle_, "morfeusz_analyse"));
		const char* dle = dlerror();
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError(
				"Error opening library (no morfeusz_analyse symbol)", dle,
				morfeusz_library_);
		}

		dlerror();
		typedef char*(*about_func_t)();
		about_func_t about_func = reinterpret_cast<about_func_t>(
				dlsym(morfeusz_lib_handle_, "morfeusz_about"));
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError(
				"Error opening library (no morfeusz_about symbol)",
				dle, morfeusz_library_);
		}
		std::string about = about_func();
		if (about.find(require_version_) == about.npos) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError("Invalid Morfeusz version. Requested " +
				require_version_ + ", got ", about, morfeusz_library_);
		}

		dlerror();
		typedef int (*opt_func_t)(int, int);
		opt_func_t opt_func = reinterpret_cast<opt_func_t>(
				dlsym(morfeusz_lib_handle_, "morfeusz_set_option"));
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError(
				"Error opening library (no morfeusz_set_option symbol)",
				dle, morfeusz_library_);
		}
		if (opt_func != NULL) {
			opt_func(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
		}
	}

	MorfeuszAnalyser* MorfeuszAnalyser::clone() const
	{
		MorfeuszAnalyser* copy = new MorfeuszAnalyser(&tagset(),
				conv_->clone(), morfeusz_library_, require_version_);
		copy->ign_tag_ = ign_tag_;
		copy->warn_on_ign_ = warn_on_ign_;
		copy->warn_on_fold_failure_ = warn_on_fold_failure_;
		return copy;
	}

	MorfeuszAnalyser::MorfeuszAnalyser(const Config::Node& cfg)
		: MorphAnalyser(cfg), conv_(NULL), ign_tag_(), warn_on_ign_(false)
	{
		std::string fn = cfg.get("converter", "");
		std::ifstream ifs;
		Path::Instance().open_stream_or_throw(fn, ifs, "converter");

		Config::Node conv_cfg = Config::from_stream(ifs);
		std::auto_ptr<Conversion::TagsetConverter> c(
				new Conversion::TagsetConverter(conv_cfg));

		if (c->tagset_to().id() != tagset().id()) {
			throw TagsetMismatch("Morfeusz analyser creation", tagset(),
					c->tagset_to());
		}
		conv_ = c.release();

		std::string ign_tag_string = cfg.get("ign_tag", "ign");
		ign_tag_ = conv_->tagset_from().parse_simple_tag(
				ign_tag_string, false);
		warn_on_ign_ = cfg.get("warn_on_ign", false);
		warn_on_fold_failure_ =  cfg.get("warn_on_fold_failure", true);

		morfeusz_library_ = cfg.get("library", "libmorfeusz.so");
		require_version_ = cfg.get("require_version", "Morfeusz SIAT");
		load_morfeusz_library();
	}

	MorfeuszAnalyser::~MorfeuszAnalyser()
	{
		dlclose(morfeusz_lib_handle_);
		delete conv_;
	}

	bool MorfeuszAnalyser::process_functional(const Toki::Token &t,
			boost::function<void(Token *)>sink)
	{
		std::string s = Toki::Util::to_utf8(t.orth());
		// Morfeusz demands a nonconst char*
		InterpMorf *ppmorf = morfeusz_analyse_handle_(
				const_cast<char*>(s.c_str()));
		std::vector<MorfeuszEdge> pmorf = morfeusz_preprocess(ppmorf);
		if (pmorf.empty()) { // no analyses
			return false;
		} else if (pmorf.size() == 1) { // only one analysis
			if (pmorf[0].lemma.length() > 0) {
				std::vector<Token*> vec;
				vec.push_back(make_token(t, pmorf[0]));
				flush_convert(vec, sink);
				return true;
			} else {
				return false;
			}
		} else { // token was split, or there are multiple analyses (lemmas)
			return process_complex_analysis(t, pmorf, sink);
		}
	}

	bool MorfeuszAnalyser::process_complex_analysis(const Toki::Token &t,
			std::vector<MorfeuszEdge>& pmorf,
			boost::function<void(Token *)>sink)
	{
		int node_count = 0;
		int edge_count = pmorf.size();
		foreach (const MorfeuszEdge& mri, pmorf) {
			node_count = std::max(node_count, mri.node_to);
		}
		// the numbering starts at 0 and we got the last valid node number
		++node_count;
		// build adjacency lists, folding lemma ambiguities
		std::vector< std::vector< int > > succ(node_count);
		std::vector< std::vector< int > > prec(node_count);
		for (int i = 0; i < edge_count; ++i) {
			MorfeuszEdge& edge = pmorf[i];
			int actual_edge_i = -1;
			foreach (int out_edge, succ[edge.node_from]) {
				if (pmorf[out_edge].node_to == edge.node_to) {
					actual_edge_i = out_edge;
				}
			}
			if (actual_edge_i >= 0) {
				// duplicate edge -- simple lemma ambiguity
				morfeusz_into_token(pmorf[actual_edge_i].token, edge);
			} else {
				edge.token = make_token(t, edge);
				succ[edge.node_from].push_back(i);
				prec[edge.node_to].push_back(i);
			}
		}

		std::vector<Token*> unambiguous;
		int current_node = 0;
		while (current_node < node_count) {
			if (succ[current_node].size() > 1) {
				// complex case, segmentation ambiguity
				if (!unambiguous.empty()) {
					flush_convert(unambiguous, sink);
					unambiguous.clear();
				}

				int merge_node = -1;
				std::vector< std::vector< Token* > > paths;
				// follow all paths to the merge point
				foreach (int tse, succ[current_node]) {
					paths.push_back(std::vector<int>());
					paths.back().push_back(tse);
					int v = pmorf[tse].node_to;
					while (prec[v].size() == 1) {
						if (succ[v].size() != 1) {
							throw MorfeuszError("path splits twice",
									t.orth_utf8(), pmorf);
						}
						tse = *succ[v].begin();
						paths.back().push_back(pmorf[tse].token);
						v = pmorf[tse].node_to;
					}
					//assume this is the merge node, check for consistency
					if (merge_node != -1 && merge_node != v) {
						throw MorfeuszError("path merge node ambiguity",
								t.orth_utf8(), pmorf);
					}
					merge_node = v;
				}

				flush_convert(paths, sink);
				current_node = merge_node;
			} else if (!succ[current_node].empty()) { //simple case, only one interp
				int edge = *succ[current_node].begin();
				unambiguous.push_back(pmorf[edge].token);
				if (pmorf[edge].node_to != current_node + 1) {
					throw MorfeuszError(
							"simple path has non-consecutive nodes",
							t.orth_utf8(), pmorf);
				}
				++current_node;
			} else { //only the last node should have no succesors
				if (current_node != node_count - 1) {
					throw MorfeuszError(
							"node without succesors that not last node",
							t.orth_utf8(), pmorf);
				}
				++current_node;
			}
		}
		if (!unambiguous.empty()) {
			flush_convert(unambiguous, sink);
		}
		return true;
	}

	Token* MorfeuszAnalyser::make_token(const Toki::Token& t,
			const MorfeuszEdge &m) const
	{
		Token* tt = new Token();
		if (m.node_from == 0) {
			tt->set_wa(t.preceeding_whitespace());
		} else {
			tt->set_wa(Toki::Whitespace::None);
		}
		morfeusz_into_token(tt, m);
		return tt;
	}

	void MorfeuszAnalyser::morfeusz_into_token(Token *tt,
			const MorfeuszEdge& m) const
	{
		tt->set_orth(m.orth);
		if (!m.tag_string.empty()) {
			conv_->tagset_from().lexemes_into_token(*tt, m.lemma,
					m.tag_string);
		} else {
			Lexeme ign_lex(m.orth, ign_tag_);
			tt->add_lexeme(ign_lex);
			if (warn_on_ign_) {
				std::cerr << "Morfeusz: tagging as ign: "
						<< ign_lex.lemma_utf8() << "\n";
			}
		}
	}

	void MorfeuszAnalyser::flush_convert(std::vector<Token *> &vec,
			boost::function<void(Token *)>sink)
	{
		conv_->convert_simple(vec, sink);
	}

	void MorfeuszAnalyser::flush_convert(
			std::vector<std::vector<Token *> > &vec,
			boost::function<void(Token *)>sink)
	{
		conv_->convert_ambiguous(vec, sink, warn_on_fold_failure_);
	}

} /* end ns Maca */
