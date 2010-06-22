#ifndef LIBPLTAGGER_SETTINGS_H
#define LIBPLTAGGER_SETTINGS_H

#include <map>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <unicode/unistr.h>
#include <ostream>

namespace PlTagger {

	/**
	 * Look for a filename under the search path and return a path to a file
	 * that exists, or an empty string in case of failure
	 */
	std::string find_file_in_search_path(const std::string& filename);

	/**
	 * Open a file stream for a file in the library search path
	 */
	bool open_file_from_search_path(const std::string& filename, std::ifstream& ifs);

	std::vector<std::string> get_library_config_path();

	std::string get_library_config_path_string();

	void set_library_config_path(const std::vector<std::string> &);

	void set_library_config_path(const std::string &);

	/**
	 * Convenience class to set the library config path and have it automatically
	 * reset to the original value upon destruction
	 */
	class LibraryConfigPathSetter
	{
	public:
		/// Constructor
		LibraryConfigPathSetter(const std::string& new_path);

		/// Destructor
		~LibraryConfigPathSetter();
	private:
		/// Stored old path
		std::vector<std::string> old_path_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SETTINGS_H
