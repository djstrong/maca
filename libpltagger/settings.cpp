#include <libpltagger/settings.h>

#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif

#include <libtoki/util/foreach.h>

#include <libtoki/parser/loose_ini_paser.h>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>

#include <libtoki/exception.h>

namespace PlTagger {

	namespace {
		static std::vector<std::string> library_config_path;

		std::string get_library_config_path_string()
		{
			return boost::algorithm::join(library_config_path, std::string(";"));
		}
	} // end anon namespace

	std::string find_file_in_search_path(const std::string &filename)
	{
		namespace fs = boost::filesystem;
		fs::path i(filename);
		foreach (const std::string& s, library_config_path) {
			fs::path pi = s / i;
			if (fs::exists(pi) && fs::is_regular(pi)) {
				return pi.string();
			}
		}
		return "";
	}

	bool open_file_from_search_path(const std::string &filename, std::ifstream &ifs)
	{
		std::string f = find_file_in_search_path(filename);
		if (!f.empty()) {
			ifs.open(f.c_str());
			return true;
		}
		return false;
	}

	std::vector<std::string> get_library_config_path()
	{
		return library_config_path;
	}

	std::string get_library_config_path_string()
	{
		return boost::algorithm::join(library_config_path, LIBPLTAGGER_PATH_SEPARATOR);
	}

	void set_library_config_path(const std::vector<std::string> &vec)
	{
		library_config_path = vec;
	}

	void set_library_config_path(const std::string &s)
	{
		std::vector<std::string> v;
		boost::algorithm::split(v, s, std::bind1st(std::equal_to<char>(), LIBPLTAGGER_PATH_SEPARATOR[0]));
		set_library_config_path(v);
	}

	namespace {
		bool init_lcp()
		{
#ifdef LIBPLTAGGER_DATA_DIR
		set_library_config_path(LIBPLTAGGER_DATA_DIR);
#else
		set_library_config_path(".");
#endif
			return !library_config_path.empty();
		}

		static bool lcp_init = init_lcp();
	} //end anon namespace

	LibraryConfigPathSetter::LibraryConfigPathSetter(const std::string &new_path)
		: old_path_(get_library_config_path())
	{
		set_library_config_path(new_path);
	}

	LibraryConfigPathSetter::~LibraryConfigPathSetter()
	{
		set_library_config_path(old_path_);
	}
} /* end namespace PlTagger */
