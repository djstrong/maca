#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/util/debug.h>
#include <libpltagger/util/settings.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/tagsetmanager.h>

#include <libtoki/util/foreach.h>

#include <boost/program_options.hpp>

#include <algorithm>
#include <fstream>
#include <iterator>

void tagset_info(const PlTagger::Tagset& tagset)
{
	std::cerr << "Tagset loaded: "
		<< tagset.pos_dictionary().size() << " POSes, "
		<< tagset.attribute_dictionary().size() << " attributes, "
		<< tagset.value_dictionary().size() << " values\n";
	std::cerr << "Size is " << tagset.size()
		<< " (extra size is " << tagset.size_extra() << ")\n";
	std::cerr << "POSes: ";
	foreach (const std::string& s, tagset.pos_dictionary()) {
		std::cerr << s << " ";
	}
	std::cerr << "\n";
}

void tagset_query_loop(const PlTagger::Tagset& tagset)
{
	while (std::cin.good()) {
		std::string s;
		std::cin >> s;
		if (!s.empty()) {
			PlTagger::pos_idx_t pos = tagset.pos_dictionary().get_id(s);
			PlTagger::attribute_idx_t atr = tagset.attribute_dictionary().get_id(s);
			PlTagger::value_idx_t val = tagset.value_dictionary().get_id(s);
			if (tagset.pos_dictionary().is_id_valid(pos)) {
				std::cout << s << " -> POS ->" ;
				foreach (PlTagger::attribute_idx_t a, tagset.get_pos_attributes(pos)) {
					std::string astr = tagset.attribute_dictionary().get_string(a);
					if (tagset.get_pos_required_attributes(pos)[a]) {
						std::cout << " " << astr;
					} else {
						std::cout << " [" << astr << "]";
					}
				}
				std::cout << "\n";
			} else if (tagset.attribute_dictionary().is_id_valid(atr)) {
				std::cout << s << " -> attribute ->";
				foreach (PlTagger::value_idx_t v, tagset.get_attribute_values(atr)) {
					std::cout << " " << tagset.value_dictionary().get_string(v);
				}
				std::cout << "\nIn POSes:";
				for (PlTagger::pos_idx_t p = (PlTagger::pos_idx_t)(0); p < tagset.pos_dictionary().size(); ++p) {
					if (tagset.get_pos_valid_attributes(p)[atr]) {
						std::cout << " " << tagset.pos_dictionary().get_string(p);
						if (!tagset.get_pos_required_attributes(p)[atr]) {
							std::cout << "?";
						}
					}
				}
				std::cout << "\n";
			} else if (tagset.value_dictionary().is_id_valid(val)) {
				PlTagger::attribute_idx_t a = tagset.get_value_attribute(val);
				std::cout << s << " -> value -> attribute ";
				std::cout << tagset.attribute_dictionary().get_string(a);
				std::cout << " .";
				foreach (PlTagger::value_idx_t v, tagset.get_attribute_values(a)) {
					std::cout << " " << tagset.value_dictionary().get_string(v);
				}
				std::cout << "\nIn POSes:";
				for (PlTagger::pos_idx_t p = (PlTagger::pos_idx_t)(0); p < tagset.pos_dictionary().size(); ++p) {
					if (tagset.get_pos_valid_attributes(p)[a]) {
						std::cout << " " << tagset.pos_dictionary().get_string(p);
						if (!tagset.get_pos_required_attributes(p)[a]) {
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
}

void tag_parse_loop(const PlTagger::Tagset& tagset, bool validate, bool sort)
{
	while (std::cin.good()) {
		std::string s;
		std::cin >> s;
		if (!s.empty()) {
			try {
				PlTagger::Token t;
				tagset.lexemes_into_token(t, UnicodeString(), s);
				std::vector<std::string> out;
				foreach (const PlTagger::Lexeme& lex, t.lexemes()) {
					std::stringstream ss;
					ss << tagset.tag_to_string(lex.tag());
					if (validate) {
						tagset.validate_tag(lex.tag(), false, &ss);
					}
					out.push_back(ss.str());
				}
				if (sort) {
					std::sort(out.begin(), out.end());
				}
				std::copy(out.begin(), out.end(),
						std::ostream_iterator<std::string>(std::cout, "\n"));
			} catch (PlTagger::TagParseError& e) {
				std::cerr << "TagParseError: " << e.info() << "\n";
			}
		}
	}
}

int main(int argc, char** argv)
{
	std::string tagset_load, tagset_save;
	bool quiet = false;
	bool parse = false, validate = false, sort = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("tagset,t", value(&tagset_load),
			 "Path to tagset ini file to load\n")
			("save-tagset,S", value(&tagset_save),
			 "Path to tagset ini file to save\n")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset\n")
			("parse,p", value(&parse)->zero_tokens(),
			 "Parse complex tag strings mode")
			("validate,v", value(&validate)->zero_tokens(),
			 "Validate parsed tags")
			("sort,s", value(&sort)->zero_tokens(),
			 "Sort parsed tags")
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

	if (!tagset_load.empty()) {
		try {
			const PlTagger::Tagset& tagset = PlTagger::get_named_tagset(tagset_load);

			if (!quiet) {
				tagset_info(tagset);
			}

			if (!tagset_save.empty()) {
				std::ofstream ofs(tagset_save.c_str());
				if (!quiet) {
					std::cerr << "Saving tagset to " << tagset_save << " ... ";
				}
				PlTagger::TagsetParser::save_ini(tagset, ofs);
				if (!quiet) {
					std::cerr << "ok\n";
				}
			}

			if (parse) {
				std::cerr << "(Tag parse mode)\n";
				tag_parse_loop(tagset, validate, sort);
			} else {
				std::cerr << "(Tagset query mode)\n";
				tagset_query_loop(tagset);
			}
		} catch (PlTagger::PlTaggerError& e) {
			std::cerr << "PlTagger Error: " << e.info() << "\n";
			return 4;
		}
	} else {
		std::cerr << "Usage: tagset-tool [OPTIONS] <tagset-file>\n";
		std::cerr << "See tagset-tool --help\n";
		return 1;
	}
	return 0;
}
