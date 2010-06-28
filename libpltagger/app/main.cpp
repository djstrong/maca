#include <libpltagger/config_d.h>
#ifdef HAVE_SFST
#include <libpltagger/morph/sfstanalyser.h>
#endif

#ifdef HAVE_MORFEUSZ
#include <libpltagger/morph/morfeuszanalyser.h>
#endif

#include <libpltagger/morph/mapanalyser.h>

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <libtoki/foreach.h>
#include <libtoki/confignode.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/conv/tagsetconverter.h>

#include <libpltagger/debug.h>
#include <fstream>

int main(int argc, char** argv)
{
	std::string sfst, mdict, conv;
	std::string tagset_load, tagset_save;
	using boost::program_options::value;
#ifdef HAVE_MORFEUSZ
	bool morfeusz;
#endif
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
#ifdef HAVE_SFST
			("sfst-transducer,s", value(&sfst)->default_value("/home/ilor/semantic/tagger/fst/M.cfst"),
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
			("save-tagset", value(&tagset_save),
			 "Path to tagset ini file to save\n")
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
			std::cerr << "Tagset loaded: "
				<< tagset->pos_dictionary().size() << " POSes, "
				<< tagset->attribute_dictionary().size() << " attributes, "
				<< tagset->value_dictionary().size() << " values\n";
			std::cerr << "Size is " << tagset->size()
				<< " (extra size is " << tagset->size_extra() << ")\n";
			//std::cerr << "POSes: ";
			//foreach (const std::string& s, tagset->pos_dictionary()) {
			//	std::cerr << s << " ";
			//}

			if (!tagset_save.empty()) {
				std::ofstream ofs(tagset_save.c_str());
				PlTagger::TagsetParser::save_ini(*tagset, ofs);
			}
		} else {
			std::cerr << "File open error\n";
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

	if (ma) {
		while (std::cin.good()) {
			std::string s;
			std::cin >> s;
			Toki::Token t(s.c_str(), "t", Toki::Whitespace::None);
			std::vector<PlTagger::Token*> tv = ma->process(t);
			foreach (PlTagger::Token* tt, tv) {
				if (tt != tv[0]) {
					std::cout << "---\n";
				}
				PlTagger::token_output(ma->tagset(), std::cout, tt);
				std::cout << "\n";
			}
		}
	}
}
