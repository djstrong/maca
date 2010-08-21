#ifndef LIBPLTAGGER_MAPANALYSER_H
#define LIBPLTAGGER_MAPANALYSER_H

#include <libpltagger/morph/morphanalyser.h>
#include <libpltagger/util/settings.h>

#include <libtoki/util/confignode.h>
#include <libtoki/util/foreach.h>

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

	/// Helper struct for ICU string caseless compare
	struct IcuStringCaselessCompare
	{
		bool operator()(const UnicodeString& u1, const UnicodeString& u2) const {
			return u1.caseCompare(u2, 0) < 0;
		}
	};

	/// Helper struct for ICU hashing
	struct IcuHash
	{
		std::size_t operator()(const UnicodeString& x) const
		{
			return x.hashCode();
		}
	};

	/// Helper struct for ICU string caseless compare and hashing
	struct IcuStringCaselessEqual
	{
		bool operator()(const UnicodeString& u1, const UnicodeString& u2) const {
			return u1.caseCompare(u2, 0) == 0;
		}

		std::size_t operator()(const UnicodeString& x) const
		{
			UnicodeString xx = x;
			xx.toLower();
			return xx.hashCode();
		}
	};


	/// typedef for a tree-map (std::map) analyser
	typedef MapAnalyser<
		std::map<
			UnicodeString,
			std::vector< std::pair<std::string, std::string>
			>
		>
	> StdMapAnalyser;

	/// typedef for a tree-map (std::map) analyser, caseless
	typedef MapAnalyser<
		std::map<
			UnicodeString,
			std::vector< std::pair<std::string, std::string> >,
			IcuStringCaselessCompare
		>
	> StdMapCaselessAnalyser;

	/// typedef for a hashmap (std::unordered_map) analyser
	typedef MapAnalyser<
		boost::unordered_map<
			UnicodeString,
			std::vector< std::pair<std::string, std::string> >,
			IcuHash
		>
	> HashMapAnalyser;

	/// typedef for a hashmap (std::unordered_map) analyser, caseless
	typedef MapAnalyser<
		boost::unordered_map<
			UnicodeString,
			std::vector< std::pair<std::string, std::string> >,
			IcuStringCaselessEqual,
			IcuStringCaselessEqual
		>
	> HashMapCaselessAnalyser;

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
		std::ifstream ifs;
		Path::Instance().open_stream_or_throw(fn, ifs, "Map analyser data");
		static const size_t BUFSIZE = 2000;
		char buf[BUFSIZE + 1];
		while (ifs.good()) {
			ifs.getline(buf, BUFSIZE);
			//std::vector< boost::iterator_range<const char*> > v;
			//std::string b(buf);
			size_t len = ifs.gcount();
			size_t i = 0;
			while (i < BUFSIZE && (buf[i] == ' ' || buf[i] == '\t')) {
				++i;
			}
			if (i + 1 < len) {
				std::vector< std::string > v;
				string_range r(buf + i, buf + len - 1); // do not include the trailing null
				boost::algorithm::split(v, r, boost::is_any_of("\t "), boost::algorithm::token_compress_on);
				if (v.size() == 3) {
					const UnicodeString& key = UnicodeString::fromUTF8(v[0]);
					map_[key].push_back(std::make_pair(v[1], v[2]));
				} else {
					std::cerr << "Invalid map line (" << v.size() << "): " << buf << "\n";
				}
			}
		}
	}

	template<typename MapT>
	bool MapAnalyser<MapT>::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		typename MapT::const_iterator i;
		const UnicodeString& key = t.orth();
		i = map_.find(key);
		if (i != map_.end()) {
			Token* tt = new Token(t);
			typedef std::pair<std::string, std::string> sp;
			foreach (const sp& o, i->second) {
				tagset().lexemes_into_token(*tt, o.first, o.second);
			}
			sink(tt);
			return true;
		} else {
			return false;
		}
	}


} /* end ns PlTagger */

#endif // LIBPLTAGGER_MAPANALYSER_H
