#include <libpltagger/morph/init.h>
#include <libpltagger/morph/constanalyser.h>
#include <libpltagger/morph/mapanalyser.h>
#include <iostream>

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

		return true;
	}

} /* end ns PlTagger */
