#ifndef LIBPLTAGGER_MAPANALYSER_H
#define LIBPLTAGGER_MAPANALYSER_H

#include <libpltagger/morph/morphanalyser.h>
#include <libtoki/confignode.h>

#include <boost/unordered_map.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace PlTagger {

	/**
	 * A standard-container-based analyser template for use with map-like
	 * containers. Data is loaded from a file in tab-separated format of
	 * orth, lemma and tag-string triplets.
	 */
	template<typename MapT>
	class MapAnalyser : public MorphAnalyser
	{
	public:
		/// Constructor for an empty analyser working with a tagset
		explicit MapAnalyser(const Tagset* tagset);

		/**
		 * Config node constructor. Recognized keys are:
		 * - data - the data file to load
		 */
		explicit MapAnalyser(const Config::Node& cfg);

		/// Data file loading function
		void load_m_dictionary(const std::string& fn);

		/// MorphAnalyser override
		bool process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);

	private:
		/// the orth to analysis map
		MapT map_;
	};

	/// typedef for a tree-map (std::map) analyser
	typedef MapAnalyser< std::map< std::string, Lexeme > > StdMapAnalyser;

	/// typedef for a hashmap (std::unordered_map) analyser
	typedef MapAnalyser< boost::unordered_map< std::string, Lexeme > > HashMapAnalyser;

/* implementation */

	template<typename MapT>
	MapAnalyser<MapT>::MapAnalyser(const Tagset* tagset)
		: MorphAnalyser(tagset), map_()
	{
	}

	template<typename MapT>
	MapAnalyser<MapT>::MapAnalyser(const Config::Node &cfg)
		: MorphAnalyser(cfg), map_()
	{
		load_m_dictionary(cfg.get<std::string>("data"));
	}

	template<typename MapT>
	void MapAnalyser<MapT>::load_m_dictionary(const std::string &fn)
	{
		std::ifstream ifs(fn.c_str());
		char buf[2001];
		while (ifs.good()) {
			ifs.getline(buf, 2000);
			//std::vector< boost::iterator_range<const char*> > v;
			//std::string b(buf);
			std::vector< std::string > v;
			boost::iterator_range<const char*> r(buf, buf + ifs.gcount());
			boost::algorithm::split(v, r, boost::is_any_of("\t"));
			if (v.size() == 3) {
				Lexeme lex(UnicodeString::fromUTF8(v[1]), Tagset().parse_simple_tag(v[2], false));
				map_.insert(std::make_pair(v[0], lex));
			}
		}
	}

	template<typename MapT>
	bool MapAnalyser<MapT>::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		typename MapT::const_iterator i;
		i = map_.find(t.orth_utf8());
		if (i != map_.end()) {
			Token* tt = new Token(t);
			tt->add_lexeme(i->second);
			sink(tt);
			return true;
		} else {
			return false;
		}
	}


} /* end ns PlTagger */

#endif // LIBPLTAGGER_MAPANALYSER_H
