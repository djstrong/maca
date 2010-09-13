#include <libmaca/morph/mapanalyser.h>

namespace Maca
{
	template<>
	const char* StdMapAnalyser::identifier = "map-case";

	template<>
	const char* StdMapCaselessAnalyser::identifier = "map";

	template<>
	const char* HashMapAnalyser::identifier = "hashmap";

	template<>
	const char* HashMapCaselessAnalyser::identifier = "hashmap-case";

	template<>
	bool StdMapAnalyser::registered =
			MorphAnalyser::register_analyser<StdMapAnalyser>();

	template<>
	bool StdMapCaselessAnalyser::registered =
			MorphAnalyser::register_analyser<StdMapCaselessAnalyser>();

	template<>
	bool HashMapAnalyser::registered =
			MorphAnalyser::register_analyser<HashMapAnalyser>();

	template<>
	bool HashMapCaselessAnalyser::registered =
			MorphAnalyser::register_analyser<HashMapCaselessAnalyser>();
}
