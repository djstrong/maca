#include <libpltagger/morph/init.h>
#include <libpltagger/morph/constanalyser.h>
#include <libpltagger/morph/mapanalyser.h>

#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libpltagger/morph/morfeuszanalyser.h>
#endif

#ifdef HAVE_SFST
#include <libpltagger/morph/sfstanalyser.h>
#endif


namespace PlTagger {

	bool init_morph_analysers()
	{
		static bool init_done = false;
		if (init_done) return false;
		MorphAnalyser::register_analyser<ConstAnalyser>("const");
		MorphAnalyser::register_analyser<StdMapAnalyser>("map-case");
		MorphAnalyser::register_analyser<StdMapCaselessAnalyser>("map");
		MorphAnalyser::register_analyser<HashMapAnalyser>("hashmap-case");
		MorphAnalyser::register_analyser<HashMapCaselessAnalyser>("hashmap");
#ifdef HAVE_SFST
		MorphAnalyser::register_analyser<SfstAnalyser>("sfst");
#endif
#ifdef HAVE_MORFEUSZ
		MorphAnalyser::register_analyser<MorfeuszAnalyser>("morfeusz");
#endif
		init_done = true;
		return true;
	}

} /* end ns PlTagger */
