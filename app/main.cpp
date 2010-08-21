#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#ifdef HAVE_SFST
#include <libpltagger/morph/sfstanalyser.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libpltagger/morph/morfeuszanalyser.h>
#endif

#include <libpltagger/morph/mapanalyser.h>
#include <libpltagger/morph/dispatchanalyser.h>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <libtoki/util/foreach.h>
#include <libtoki/util/confignode.h>
#include <libpltagger/util/settings.h>
#include <libpltagger/tagsetmanager.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/conv/tagsetconverter.h>

#include <libpltagger/io/plain.h>
#include <libpltagger/io/xces.h>
#include <libpltagger/io/xceswriter.h>

#include <libpltagger/util/debug.h>
#include <fstream>

int main(int argc, char** argv)
{
	std::string sfst, mdict, conv, cfg_analyser;
	std::string tagset_load;
	using boost::program_options::value;
#ifdef HAVE_MORFEUSZ
        bool morfeusz = false;
#endif
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
#ifdef HAVE_SFST
			("sfst-transducer,s", value(&sfst),
			 "SFST transducer file to use (compact format)")
#endif
			("m-dict-file,m", value(&mdict),
			 "M-style dictionary file to use (orth\\tlemma\\ttag)\n")
#ifdef HAVE_MORFEUSZ
			("morfeusz,M", value(&morfeusz)->zero_tokens(),
			 "Morfeusz\n")
#endif
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("convert,c", value(&conv),
			 "Tagset conversion testing\n")
			("analyser,a", value(&cfg_analyser),
			 "Analyser config file\n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}

        boost::shared_ptr<PlTagger::Conversion::TagsetConverter> converter;

        if (!conv.empty()) {
		Toki::Config::Node cfg = Toki::Config::from_file(conv);
		converter.reset(new PlTagger::Conversion::TagsetConverter(cfg));
	}
#ifdef HAVE_MORFEUSZ
	if (converter && !morfeusz) {
#else
	if (converter) {
#endif

		std::vector<PlTagger::Token*> v;
		while (std::cin.good()) {
			std::string orth;
			std::string intag;
			std::cin >> orth >> intag;
			if (!orth.empty() && ! intag.empty()) {
				UnicodeString uorth = UnicodeString::fromUTF8(orth);
				PlTagger::Tag tag;
				tag = converter->tagset_from().parse_simple_tag(intag, true);
				std::cout << converter->tagset_from().tag_to_string(tag);
				PlTagger::Lexeme lex(uorth, tag);
				PlTagger::Token* tok = new PlTagger::Token(uorth, Toki::Whitespace::None);
				tok->add_lexeme(lex);
				v.push_back(tok);
			}
		}
		converter->convert_simple(v, boost::bind(
				PlTagger::token_output,
				boost::cref(converter->tagset_to()),
				boost::ref(std::cout),
				_1));
	}

	boost::shared_ptr<PlTagger::Tagset> tagset(new PlTagger::Tagset);

	if (!tagset_load.empty()) {
		std::ifstream ifs(tagset_load.c_str());
		if (ifs.good()) {
			try {
				*tagset = PlTagger::TagsetParser::load_ini(ifs);
			} catch (PlTagger::TagsetParseError& e) {
				std::cerr << e.info() << "\n";
				exit(1);
			}
		} else {
			std::cerr << "tagset file open error\n";
			return 7;
		}
	}

	boost::shared_ptr<PlTagger::MorphAnalyser> ma;
#ifdef HAVE_MORFEUSZ
	if (morfeusz) {
		ma.reset(new PlTagger::MorfeuszAnalyser(&converter->tagset_to(), converter.get()));
	} else
#endif
#ifdef HAVE_SFST
	if (!sfst.empty()) {
		ma.reset(new PlTagger::SfstAnalyser(tagset.get(), sfst));
	} else
#endif
	if (!mdict.empty()) {
		PlTagger::HashMapAnalyser* hma = new PlTagger::HashMapAnalyser(tagset.get());
		ma.reset(hma);
		hma->load_m_dictionary(mdict);
		std::cout << "loading done\n";
	}
	if (!cfg_analyser.empty()) {
		std::ifstream ifs;
		if (PlTagger::Path::Instance().open_stream(cfg_analyser, ifs)) {
			Toki::Config::Node cfg = Toki::Config::from_stream(ifs);
			ma.reset(new PlTagger::DispatchAnalyser(cfg));
		} else {
			std::cerr << "Config file not found: " << cfg_analyser << "\n";
			return 11;
		}
	}

	std::cerr << "Nothing to do! Try --help.\n";
}
