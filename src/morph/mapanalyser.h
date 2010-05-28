#ifndef LIBPLTAGGER_MAPANALYSER_H
#define LIBPLTAGGER_MAPANALYSER_H

#include "morphanalyser.h"

#include <map>
#include <boost/unordered_map.hpp>

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

		Token* process(const Toki::Token &t);

	private:
		MapT map_;
	};

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
