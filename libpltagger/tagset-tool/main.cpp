#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/debug.h>
#include <libpltagger/settings.h>
#include <libpltagger/tagsetparser.h>

#include <libtoki/foreach.h>

#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string tagset_load, tagset_save;
	bool quiet = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("save-tagset,s", value(&tagset_save),
			 "Path to tagset ini file to save\n")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset\n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("tagset", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
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
			if (!quiet) {
				std::cerr << "Tagset loaded: "
					<< tagset->pos_dictionary().size() << " POSes, "
					<< tagset->attribute_dictionary().size() << " attributes, "
					<< tagset->value_dictionary().size() << " values\n";
				std::cerr << "Size is " << tagset->size()
					<< " (extra size is " << tagset->size_extra() << ")\n";
				std::cerr << "POSes: ";
				foreach (const std::string& s, tagset->pos_dictionary()) {
					std::cerr << s << " ";
				}
				std::cerr << "\n";
			}

			if (!tagset_save.empty()) {
				std::ofstream ofs(tagset_save.c_str());
				if (!quiet) {
					std::cerr << "Saving tagset to " << tagset_save << " ... ";
				}
				PlTagger::TagsetParser::save_ini(*tagset, ofs);
				if (!quiet) {
					std::cerr << "ok\n";
				}
			}

			while (std::cin.good()) {
				std::string s;
				std::cin >> s;
				if (!s.empty()) {
					PlTagger::pos_idx_t pos = tagset->pos_dictionary().get_id(s);
					PlTagger::attribute_idx_t atr = tagset->attribute_dictionary().get_id(s);
					PlTagger::value_idx_t val = tagset->value_dictionary().get_id(s);
					if (tagset->pos_dictionary().is_id_valid(pos)) {
						std::cout << s << " -> POS ->" ;
						foreach (PlTagger::attribute_idx_t a, tagset->get_pos_attributes(pos)) {
							std::string astr = tagset->attribute_dictionary().get_string(a);
							if (tagset->get_pos_required_attributes(pos)[a]) {
								std::cout << " " << astr;
							} else {
								std::cout << " [" << astr << "]";
							}
						}
						std::cout << "\n";
					} else if (tagset->attribute_dictionary().is_id_valid(atr)) {
						std::cout << s << " -> attribute ->";
						foreach (PlTagger::value_idx_t v, tagset->get_attribute_values(atr)) {
							std::cout << " " << tagset->value_dictionary().get_string(v);
						}
						std::cout << "\nIn POSes:";
						for (PlTagger::pos_idx_t p = (PlTagger::pos_idx_t)(0); p < tagset->pos_dictionary().size(); ++p) {
							if (tagset->get_pos_valid_attributes(p)[atr]) {
								std::cout << " " << tagset->pos_dictionary().get_string(p);
								if (!tagset->get_pos_required_attributes(p)[atr]) {
									std::cout << "?";
								}
							}
						}
						std::cout << "\n";
					} else if (tagset->value_dictionary().is_id_valid(val)) {
						PlTagger::attribute_idx_t a = tagset->get_value_attribute(val);
						std::cout << s << " -> value -> attribute ";
						std::cout << tagset->attribute_dictionary().get_string(a);
						std::cout << " ->";
						foreach (PlTagger::value_idx_t v, tagset->get_attribute_values(a)) {
							std::cout << " " << tagset->value_dictionary().get_string(v);
						}
						std::cout << "\nIn POSes:";
						for (PlTagger::pos_idx_t p = (PlTagger::pos_idx_t)(0); p < tagset->pos_dictionary().size(); ++p) {
							if (tagset->get_pos_valid_attributes(p)[a]) {
								std::cout << " " << tagset->pos_dictionary().get_string(p);
								if (!tagset->get_pos_required_attributes(p)[a]) {
									std::cout << "?";
								}
							}
						}
						std::cout << "\n";
					} else {
						std::cout << "Not recognized: " << s << "\n";
					}
				}
			}
		} else {
			std::cerr << "tagset file open error\n";
			return 7;
		}
	} else {
		std::cerr << "Usage: tagset-tool [OPTIONS] <tagset-file>\n";
		std::cerr << "See tagset-tool --help\n";
		return 1;
	}
	return 0;
}
