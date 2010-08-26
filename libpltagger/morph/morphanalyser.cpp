#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/util/settings.h>
#include <libpltagger/tagsetmanager.h>

#include <libtoki/util/foreach.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <memory>
#include <dlfcn.h>

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

	std::vector<Token*> MorphAnalyser::process_dispose(const std::vector<Toki::Token*>& t)
	{
		std::vector<Token*> v;
		process_dispose(t, v);
		return v;
	}

	void MorphAnalyser::process_dispose(const std::vector<Toki::Token*>& t, std::vector<Token*>& v)
	{
		foreach (Toki::Token* tt, t) {
			process(*tt, v);
			delete tt;
		}
	}

	Sentence* MorphAnalyser::process_dispose(Toki::Sentence* t)
	{
		std::auto_ptr<Sentence> s(new Sentence);
		process_dispose(t, s.get());
		return s.release();
	}

	void MorphAnalyser::process_dispose(Toki::Sentence* t, Sentence* v)
	{
		foreach (Toki::Token* tt, t->tokens()) {
			process(*tt, v->tokens());
			delete tt;
		}
		t->tokens().clear();
	}

	MorphAnalyser* MorphAnalyser::create(std::string class_id, const Config::Node& props)
	{
		return MorphAnalyserFactory::Instance().CreateObject(class_id, props);
	}

	std::vector<std::string> MorphAnalyser::available_analyser_types()
	{
		return MorphAnalyserFactory::Instance().RegisteredIds();
	}

	bool MorphAnalyser::load_plugin(const std::string& name, bool quiet)
	{
		std::string soname;
		if (name.size() > 1 && name.find('/') != name.npos) {
			soname = name;
		} else {
			soname = "libpltagger_" + name + ".so";
		}
		size_t count = available_analyser_types().size();
		void* handle = dlopen(soname.c_str(), RTLD_NOW | RTLD_NOLOAD);
		if (handle != NULL) {
			if (!quiet) {
				std::cerr << "Warning: " << soname << " has already been loaded\n";
			}
			return false;
		}
		handle = dlopen(soname.c_str(), RTLD_NOW);
		if (handle == NULL) {
			const char* dle = dlerror();
			if (!quiet) {
				std::cerr << "Error: dlopen error while loading " << soname << ": ";
				if (dle != NULL) {
					std::cerr << dle << "\n";
				}
			}
			return false;
		} else if (count >= available_analyser_types().size()) {
			if (!quiet) {
				std::cerr << "Warning: Plugin '" << name
						<< "' did not register any new analyser types\n";
			}
			return false;
		} else {
			if (!quiet && -Path::Instance().get_verbose()) {
				std::cerr << "Loaded plugin '" << name << "'\n";
			}
			return true;
		}
	}

} /* end ns PlTagger */
