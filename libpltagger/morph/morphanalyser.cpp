#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/morph/init.h>
#include <libpltagger/tagsetmanager.h>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <libtoki/util/foreach.h>

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

	MorphAnalyser* MorphAnalyser::create(std::string class_id, const Config::Node& props)
	{
		return MorphAnalyserFactory::Instance().CreateObject(class_id, props);
	}

	std::vector<std::string> MorphAnalyser::available_analyser_types()
	{
		return MorphAnalyserFactory::Instance().RegisteredIds();
	}

	static bool registered = init_morph_analysers();

} /* end ns PlTagger */
