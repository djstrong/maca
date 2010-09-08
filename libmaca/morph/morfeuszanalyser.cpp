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

	bool MorfeuszAnalyser::registered = MorphAnalyser::register_analyser<MorfeuszAnalyser>();

	namespace {
		boost::mutex morfeusz_mutex_;
	}



	std::vector<MorfeuszResultItem> morfeusz_preprocess(InterpMorf* pmorf)
	{
		std::vector<MorfeuszResultItem> v;
		int i = 0;
		while (pmorf[i].p != -1) {
			MorfeuszResultItem mri;
			mri.p = pmorf[i].p;
			mri.k = pmorf[i].k;
			mri.orth = UnicodeString::fromUTF8(pmorf[i].forma);
			if (pmorf[i].haslo) {
				mri.lemma = UnicodeString::fromUTF8(pmorf[i].haslo);
			}
			if (pmorf[i].interp) {
				mri.tag_string = pmorf[i].interp;
			}
			v.push_back(mri);
			++i;
		}
		return v;
	}

	std::vector<MorfeuszResultItem> morfeusz_safe_analyse(const UnicodeString& orth)
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
			const std::string input, const std::vector<MorfeuszResultItem>& interp)
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

	MorfeuszAnalyser::MorfeuszAnalyser(const Tagset* tagset,
			Conversion::TagsetConverter* conv, const std::string& libname,
			const std::string& require_version)
		: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(true)
		, morfeusz_library_(libname), require_version_(require_version)
	{
		if (conv_->tagset_to().id() != tagset->id()) throw TagsetMismatch("Morfeusz analyser creation", *tagset, conv->tagset_to());
		load_morfeusz_library();
	}

	void MorfeuszAnalyser::load_morfeusz_library()
	{
		morfeusz_lib_handle_ = dlmopen(LM_ID_NEWLM, morfeusz_library_.c_str(), RTLD_NOW);
		if (morfeusz_lib_handle_ == NULL) {
			const char* dle = dlerror();
			if (dle != NULL) {
				throw MorfeuszInitError("Error opening library (no handle)", dle, morfeusz_library_);
			}
		}

		dlerror();
		morfeusz_analyse_handle_ = (morfeusz_func_t)
				dlsym(morfeusz_lib_handle_, "morfeusz_analyse");
		const char* dle = dlerror();
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError("Error opening library (no morfeusz_analyse symbol)",
				dle, morfeusz_library_);
		}

		dlerror();
		typedef char*(*about_func_t)();
		about_func_t about_func = (about_func_t)dlsym(morfeusz_lib_handle_, "morfeusz_about");
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError("Error opening library (no morfeusz_about symbol)",
				dle, morfeusz_library_);
		}
		std::string about = about_func();
		if (about.find(require_version_) == about.npos) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError("Invalid Morfeusz version. Requested " + require_version_ + ", got ",
				about, morfeusz_library_);
		}

		dlerror();
		typedef int (*opt_func_t)(int, int);
		opt_func_t opt_func = (opt_func_t)dlsym(morfeusz_lib_handle_, "morfeusz_set_option");
		if (dle != NULL) {
			dlclose(morfeusz_lib_handle_);
			throw MorfeuszInitError("Error opening library (no morfeusz_set_option symbol)",
				dle, morfeusz_library_);

		}
		if (opt_func != NULL) {
			opt_func(MORFOPT_ENCODING, MORFEUSZ_UTF_8);
		}
	}

	MorfeuszAnalyser* MorfeuszAnalyser::clone() const
	{
		MorfeuszAnalyser* copy = new MorfeuszAnalyser(
				&tagset(), conv_->clone(), morfeusz_library_, require_version_);
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
		std::auto_ptr<Conversion::TagsetConverter> c(new Conversion::TagsetConverter(conv_cfg));

		if (c->tagset_to().id() != tagset().id()) throw TagsetMismatch("Morfeusz analyser creation", tagset(), c->tagset_to());
		conv_ = c.release();

		std::string ign_tag_string = cfg.get("ign_tag", "ign");
		ign_tag_ = conv_->tagset_from().parse_simple_tag(ign_tag_string, false);
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

	bool MorfeuszAnalyser::process_functional(const Toki::Token &t, boost::function<void(Token *)>sink)
	{
		std::vector<MorfeuszResultItem> pmorf;
		std::string s = Toki::Util::to_utf8(t.orth());
		//boost::mutex::scoped_lock lock(morfeusz_mutex_);
		InterpMorf *ppmorf = morfeusz_analyse_handle_(const_cast<char*>(s.c_str()));
		pmorf = morfeusz_preprocess(ppmorf);
		//std::vector<MorfeuszResultItem> pmorf = morfeusz_safe_analyse(t.orth());
		if (pmorf.empty()) { // no analyses
			return false;
		} else if (pmorf.size() == 1) { // only one analysis
			if (pmorf[0].lemma.length() > 0) {
				std::vector<Token*> vec;
				vec.push_back(make_token(t, pmorf[0]));
				flush_convert(vec, sink);
			} else {
				return false;
			}
		} else { // token was split, or there are multiple analyses (lemmas)
			int node_count = 0;
			int edge_count = pmorf.size();
			foreach (const MorfeuszResultItem& mri, pmorf) {
				node_count = std::max(node_count, mri.k);
			}
			++node_count; // the numbering starts at 0 and we got the last valid node number
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
								throw MorfeuszError("path splits twice", t.orth_utf8(), pmorf);
							}
							tse = succ[v][0];
							paths.back().push_back(tse);
							v = pmorf[tse].k;
						}
						//assume this is the merge node, check for consistency
						if (merge_node != -1 && merge_node != v) {
							throw MorfeuszError("path merge node ambiguity", t.orth_utf8(), pmorf);
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
							MorfeuszResultItem& mri = pmorf[ei];
							std::pair<int, int> idx = std::make_pair(mri.p, mri.k);
							alt_map_t::iterator it = alt_map.find(idx);
							if (it == alt_map.end()) {
								if (!new_path_added) {
									token_paths.push_back(std::vector<Token*>());
									new_path_added = true;
								}
								token_paths.back().push_back(make_token(t, mri));
								alt_map.insert(std::make_pair(idx, token_paths.back().back()));
							} else {
								morfeusz_into_token(it->second, mri);
							}
						}
					}

					flush_convert(token_paths, sink);
					i = merge_node;
				} else if (!succ[i].empty()) { //simple case, only one interp
					int edge = succ[i][0];
					unambiguous.push_back(make_token(t, pmorf[edge]));
					if (pmorf[edge].k != i + 1) {
						throw MorfeuszError("simple path has non-consecutive nodes", t.orth_utf8(), pmorf);
					}
					++i;
				} else { //only the last node should have no succesors
					if (i != node_count - 1) {
						throw MorfeuszError("node without succesors that is not the last node", t.orth_utf8(), pmorf);
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

	Token* MorfeuszAnalyser::make_token(const Toki::Token& t, const MorfeuszResultItem &m) const
	{
		Token* tt = new Token();
		if (m.p == 0) {
			tt->set_wa(t.preceeding_whitespace());
		} else {
			tt->set_wa(Toki::Whitespace::None);
		}
		morfeusz_into_token(tt, m);
		return tt;
	}

	void MorfeuszAnalyser::morfeusz_into_token(Token *tt, const MorfeuszResultItem& m) const
	{
		tt->set_orth(m.orth);
		if (!m.tag_string.empty()) {
			conv_->tagset_from().lexemes_into_token(*tt, m.lemma, m.tag_string);
		} else {
			Lexeme ign_lex(m.orth, ign_tag_);
			tt->add_lexeme(ign_lex);
			if (warn_on_ign_) {
				std::cerr << "Morfeusz: tagging as ign: " << ign_lex.lemma_utf8() << "\n";
			}
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
