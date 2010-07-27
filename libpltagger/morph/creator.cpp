#include <libpltagger/morph/creator.h>
#include <libpltagger/morph/dispatchanalyser.h>
#include <boost/algorithm/string.hpp>
#include <libpltagger/morph/constanalyser.h>
#include <libpltagger/tagsetmanager.h>
#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libpltagger/morph/morfeuszanalyser.h>
#endif
#ifdef HAVE_SFST
#include <libpltagger/morph/sfstanalyser.h>
#endif
#include <libpltagger/morph/mapanalyser.h>

namespace PlTagger {


	class MaCreator
	{
	public:
		MaCreator(const Config::Node& cfg);

		MorphAnalyser* get_ma(const std::string& id);

		DispatchAnalyser* create();

		const Config::Node& cfg;

		std::map<std::string, MorphAnalyser*> amap;

		const Tagset* tagset;
	};

	MaCreator::MaCreator(const Config::Node &cfg)
		: cfg(cfg), amap(), tagset(NULL)
	{
		try {
			std::string tagset_id = cfg.get("general.tagset", "");
			tagset = &get_named_tagset(tagset_id);
		} catch (boost::property_tree::ptree_error& e) {
			throw PlTaggerError("No tagset specified in the general section");
		}
	}

	MorphAnalyser* MaCreator::get_ma(const std::string &id)
	{
		std::map<std::string, MorphAnalyser*>::iterator i;
		i = amap.find(id);
		if (i != amap.end()) {
			return i->second;
		} else {
			const Config::Node* cfgp(NULL);
			try {
				cfgp = &cfg.get_child("ma:" + id);
			} catch (boost::property_tree::ptree_error& e) {
				throw PlTaggerError("Morph analyser identifier not found in config");
			}
			MorphAnalyser* ma = make_analyser(*cfgp);
			if (ma->tagset().id() != tagset->id()) {
				std::auto_ptr<MorphAnalyser> aptr(ma);
				throw TagsetMismatch("Morph analyser creation : " + id, *tagset, ma->tagset());
			}
			amap.insert(std::make_pair(id, ma));
			return ma;
		}
	}

	MorphAnalyser* make_analyser(const Config::Node& cfg)
	{
		MorphAnalyser* ma(NULL);
		std::string id = cfg.get("class", "");
		if (id == "const") {
			ma = new ConstAnalyser(cfg);
#ifdef HAVE_SFST
		} else if (id == "sfst") {
			ma = new SfstAnalyser(cfg);
#endif
		} else if (id == "map") {
			ma = new StdMapAnalyser(cfg);
		} else if (id == "hashmap") {
			ma = new HashMapAnalyser(cfg);
#ifdef HAVE_MORFEUSZ
		} else if (id == "morfeusz") {
			ma = new MorfeuszAnalyser(cfg);
#endif
		} else {
			throw PlTaggerError("Unknown analyser type: " + id);
		}
		return ma;
	}

	DispatchAnalyser* MaCreator::create()
	{
		const Config::Node* dnp = NULL;
		DispatchAnalyser* da = new DispatchAnalyser(tagset);
		try {
			dnp = &cfg.get_child("default");
		} catch (boost::property_tree::ptree_error& e) {
			throw PlTaggerError("No default MA section in config");
		}
		const Config::Node& dn = *dnp;

		foreach (const Config::Node::value_type &v, dn) {
			if (v.first == "ma") {
				MorphAnalyser* ma = get_ma(v.second.data());
				da->add_default_handler(ma);
			}
		}

		foreach (const Config::Node::value_type &v, cfg) {
			if (v.first == "rule") {
				std::vector<std::string> ttv;
				foreach (const Config::Node::value_type &vv, v.second) {
					if (vv.first == "toki_type") {
						boost::algorithm::split(ttv, vv.second.data(), boost::is_any_of(" ,"));
					}
				}
				if (ttv.empty()) {
					throw PlTaggerError("No toki_type specified in rule");
				}
				foreach (const Config::Node::value_type &vv, v.second) {
					if (vv.first == "ma") {
						MorphAnalyser* ma = get_ma(vv.second.data());
						foreach (const std::string& s, ttv) {
							da->add_type_handler(s, ma);
						}
					}
				}
			}
		}
		return da;
	}

	MorphAnalyser* create_analyser(const Config::Node &cfg)
	{
		MaCreator mc(cfg);
		return mc.create();
	}

} /* end ns PlTagger */
