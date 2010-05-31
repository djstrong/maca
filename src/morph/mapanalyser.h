#ifndef LIBPLTAGGER_MAPANALYSER_H
#define LIBPLTAGGER_MAPANALYSER_H

#include "morphanalyser.h"

#include <map>
#include <boost/unordered_map.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace PlTagger {

	template<typename MapT>
	class MapAnalyser : public MorphAnalyser
	{
	public:
		MapAnalyser()
			: map_()
		{
		}

		~MapAnalyser()
		{
		}

		void load_m_dictionary(const std::string& fn);

		Token* process(const Toki::Token &t);

	private:
		MapT map_;
	};

	template<typename MapT>
	void MapAnalyser<MapT>::load_m_dictionary(const std::string &fn)
	{
		std::ifstream ifs(fn.c_str());
		char buf[201];
		while (ifs.good()) {
			ifs.getline(buf, 200);
			//std::vector< boost::iterator_range<const char*> > v;
			//std::string b(buf);
			std::vector< std::string > v;
			boost::iterator_range<const char*> r(buf, buf + ifs.gcount());
			boost::algorithm::split(v, r, boost::is_any_of("\t"));
			if (v.size() == 3) {
				Lexeme lex(UnicodeString::fromUTF8(v[1]), Tag(v[2]));
				map_.insert(std::make_pair(v[0], lex));
			}
		}
	}

	template<typename MapT>
	Token* MapAnalyser<MapT>::process(const Toki::Token &t)
	{
		typename MapT::const_iterator i;
		i = map_.find(t.orth_utf8());
		if (i != map_.end()) {
			Token* tt = new Token(t);
			tt->add_lexeme(i->second);
			return tt;
		} else {
			return NULL;
		}
	}

	typedef MapAnalyser< std::map< std::string, Lexeme > > StdMapAnalyser;

	typedef MapAnalyser< boost::unordered_map< std::string, Lexeme > > HashMapAnalyser;

} /* end ns PlTagger */

#endif // LIBPLTAGGER_MAPANALYSER_H
