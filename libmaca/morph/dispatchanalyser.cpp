#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/morph/constanalyser.h>
#include <libmaca/util/settings.h>

#include <libtoki/util/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>

namespace Maca {

	DispatchAnalyser::DispatchAnalyser(const Tagset* tagset)
		: MorphAnalyser(tagset), type_handlers_(), analysers_(), default_(), fallback_(NULL)
	{
	}

	boost::shared_ptr<DispatchAnalyser> DispatchAnalyser::create_from_named_config(const std::string& config_name)
	{
		const Config::Node& cfg = get_named_config(config_name);
		return boost::make_shared<DispatchAnalyser>(cfg);
	}

	namespace {
		/**
		 * Helper class to cache morph analysers with the same id for the duration
		 * of the construction of a DispatchAnalyser from a config object
		 */
		class MaCreator
		{
		public:
			/// Constructor
			MaCreator(const Tagset& tagset, const Config::Node& cfg);

			/// Destructor, disposes of all created analysers unless okay was called
			~MaCreator();

			/// caching getter for a morph analyser with a given id
			MorphAnalyser* get_ma(const std::string& id, bool autoload);

			/// Okay flag setter (do not dispose of created analysers at destruction)
			void okay();
		private:
			/// The config object
			const Config::Node& cfg_;

			/// Analyser by-id cache
			std::map<std::string, MorphAnalyser*> amap_;

			/// The tagset all analysers should output
			const Tagset& tagset_;

			/// okay flag
			bool okay_;
		};

		MaCreator::MaCreator(const Tagset& tagset, const Config::Node &cfg)
			: cfg_(cfg), amap_(), tagset_(tagset), okay_(false)
		{
		}

		MaCreator::~MaCreator()
		{
			if (!okay_) {
				typedef std::map<std::string, MorphAnalyser*>::value_type vt;
				foreach (vt& v, amap_) {
					delete v.second;
				}
			}
		}

		void MaCreator::okay()
		{
			okay_ = true;
		}
	} /* end anon ns */

	MorphAnalyser* MaCreator::get_ma(const std::string &id, bool autoload)
	{
		std::map<std::string, MorphAnalyser*>::iterator i;
		i = amap_.find(id);
		if (i != amap_.end()) {
			return i->second;
		} else {
			const Config::Node* cfgp(NULL);
			try {
				cfgp = &cfg_.get_child("ma:" + id);
			} catch (boost::property_tree::ptree_error& e) {
				throw MacaError("Morph analyser identifier not found in config");
			}
			std::string id = cfgp->get("class", "");
			MorphAnalyser* ma(NULL);
			try {
				ma = MorphAnalyser::create(id, *cfgp);
			} catch (MorphAnalyserFactoryException&) {
				if (autoload) {
					if (MorphAnalyser::load_plugin(id, false)) {
						try {
							ma = MorphAnalyser::create(id, *cfgp);
						} catch (MorphAnalyserFactoryException&) {
							throw MacaError("Unknown analyser type: " + id + " (plugin found but create failed)");
						}
					} else {
						throw MacaError("Unknown analyser type: " + id + " (plugin not found)");
					}
				} else {
					throw MacaError("Unknown analyser type: " + id);
				}
			}

			if (ma->tagset().id() != tagset_.id()) {
				std::auto_ptr<MorphAnalyser> aptr(ma);
				throw TagsetMismatch("Morph analyser creation : " + id, tagset_, ma->tagset());
			}
			amap_.insert(std::make_pair(id, ma));
			return ma;
		}
	}

	DispatchAnalyser::DispatchAnalyser(const Config::Node &cfg)
		: MorphAnalyser(cfg.get_child("general")), type_handlers_(), analysers_(), default_(), fallback_(NULL)
	{
		const Config::Node* dnp = NULL;
		try {
			dnp = &cfg.get_child("default");
		} catch (boost::property_tree::ptree_error& e) {
			throw MacaError("No default MA section in config");
		}

		std::string ign_tag_string = cfg.get("general.ign_tag", "");
		if (!ign_tag_string.empty()) {
			fallback_ = new ConstAnalyser(&tagset(), ign_tag_string);
		}

		const Config::Node* dng = NULL;
		try {
			dng = &cfg.get_child("general");
		} catch (boost::property_tree::ptree_error& e) {
		}
		if (dng != NULL) {
			foreach (const Config::Node::value_type &v, *dng) {
				if (v.first == "plugin") {
					MorphAnalyser::load_plugin(v.second.data(), false);
				}
			}
		}

		bool autoload = cfg.get("general.plugin_autoload", true);

		MaCreator mc(tagset(), cfg);

		foreach (const Config::Node::value_type &v, *dnp) {
			if (v.first == "ma") {
				MorphAnalyser* ma = mc.get_ma(v.second.data(), autoload);
				add_default_handler(ma);
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
					throw MacaError("No toki_type specified in rule");
				}
				foreach (const Config::Node::value_type &vv, v.second) {
					if (vv.first == "ma") {
						MorphAnalyser* ma = mc.get_ma(vv.second.data(), autoload);
						foreach (const std::string& s, ttv) {
							add_type_handler(s, ma);
						}
					}
				}
			}
		}

		mc.okay();
	}

	DispatchAnalyser::~DispatchAnalyser()
	{
		foreach (MorphAnalyser* ma, analysers_) {
			delete ma;
		}
		delete fallback_;
	}

	DispatchAnalyser* DispatchAnalyser::clone() const
	{
		DispatchAnalyser* copy = new DispatchAnalyser(&tagset());
		std::map<MorphAnalyser*, MorphAnalyser*> clones;
		foreach (MorphAnalyser* m, analysers_) {
			clones[m] = m->clone();
			copy->analysers_.insert(clones[m]);
		}
		copy->type_handlers_ = type_handlers_;
		foreach (analyser_map_t::value_type& v, copy->type_handlers_) {
			foreach (MorphAnalyser*& m, v.second) {
				m = clones[m];
				assert(m);
			}
		}
		copy->default_ = default_;
		foreach (MorphAnalyser*& m, copy->default_) {
			m = clones[m];
			assert(m);
		}
		if (fallback_) {
			copy->fallback_ = fallback_->clone();
		}
		return copy;
	}

	void DispatchAnalyser::add_type_handler(const std::string &type, MorphAnalyser *a)
	{
		if (a->tagset().id() != tagset().id()) throw TagsetMismatch("dispatch analyser handler", tagset(), a->tagset());
		analysers_.insert(a);
		type_handlers_[type].push_back(a);
	}

	bool DispatchAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		std::map<std::string, std::vector<MorphAnalyser*> >::const_iterator i;
		i = type_handlers_.find(t.type());
		const std::vector<MorphAnalyser*>* v = &default_;
		if (i != type_handlers_.end()) {
			v = &i->second;
		}
		foreach (MorphAnalyser* ma, *v) {
			if (ma->process_functional(t, sink)) {
				return true;
			}
		}
		if (fallback_) {
			return fallback_->process_functional(t, sink);
		} else {
			throw MacaError("Token was not processed by any of the analysers and there is no fallback: " + t.orth_utf8() + "");
			return false;
		}
	}

	void DispatchAnalyser::add_default_handler(MorphAnalyser* a)
	{
		default_.push_back(a);
		analysers_.insert(a);
	}

	size_t DispatchAnalyser::default_handlers_count() const
	{
		return default_.size();
	}

	size_t DispatchAnalyser::handler_count() const
	{
		return type_handlers_.size();
	}


} /* end ns Maca */
