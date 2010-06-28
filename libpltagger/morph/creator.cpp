#include <libpltagger/morph/creator.h>
#include <libpltagger/morph/dispatchanalyser.h>
#include <boost/algorithm/string.hpp>
#include <libpltagger/morph/constanalyser.h>
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

	MorphAnalyser* create_analyser(const Toki::Config::Node& cfg)
	{
		DispatchAnalyser* da(NULL);
		MorphAnalyser* ma(NULL);
		foreach (const Toki::Config::Node::value_type &v, cfg) {
			if (v.first == "const") {
				ma = new ConstAnalyser(v.second);
#ifdef HAVE_SFST
			} else if (v.first == "sfst") {
				ma = new SfstAnalyser(v.second);
#endif
			} else if (v.first == "map") {
				ma = new StdMapAnalyser(v.second);
			} else if (v.first == "hashmap") {
				ma = new HashMapAnalyser(v.second);
#ifdef HAVE_MORFEUSZ
			} else if (v.first == "morfeusz") {
				ma = new MorfeuszAnalyser(v.second);
#endif
			} else {
				throw 9;
			}
			std::string toki_types = v.second.get("toki_type", "");
			string_range_vector sr;
			boost::algorithm::split(sr, toki_types, boost::is_any_of(", "));
			if (da == NULL) {
				da = new DispatchAnalyser(&ma->tagset());
			}
			if (sr.empty()) {
				if (da->default_handler() == NULL) {
					da->set_default_handler(ma);
				} else {
					throw 9;
				}
			} else {
				foreach (const string_range& r, sr) {
					if (!r.empty()) {
						da->add_type_handler(boost::copy_range<std::string>(r), ma);
					}
				}
			}
		}
		if (da->handler_count() == 0) {
			return ma;
		} else {
			return da;
		}
	}

} /* end ns PlTagger */
