#ifndef LIBPLTAGGER_TAGSET_H
#define LIBPLTAGGER_TAGSET_H

#include <libpltagger/tag.h>

#include <set>
#include <map>
#include <vector>

#include <boost/range.hpp>
#include <boost/function.hpp>

#include <libpltagger/exception.h>
#include <libpltagger/symboldictionary.h>
#include <libpltagger/typedefs.h>

namespace PlTagger {

	/**
	 * Exception class for signalling tag parse errors
	 */
	class TagParseError : public PlTaggerError
	{
	public:
		TagParseError(const std::string &what)
		 : PlTaggerError(what)
		{
		}

		~TagParseError() throw()
		{
		}
	};

	class TagsetParser;

	class Tagset
	{
	public:
		Tagset();

		/**
		 * Tagset convenience constructor, parse a string as if it were the
		 * contents of a tagset ini file
		 */
		explicit Tagset(const char*);

		/**
		 * Tag parsing -- functional version, whole tag string.
		 *
		 * A simple wrapper for string split and a call to the split string version.
		 */
		void parse_tag(const string_range& s, bool allow_extra, boost::function<void (const Tag&)> sink) const;

		/**
		 * Tag parsing -- functional version, whole tag string, char* overload.
		 *
		 * A simple wrapper for string split and a call to the split string version.
		 */
		void parse_tag(const char* c, bool allow_extra, boost::function<void (const Tag&)> sink) const {
			parse_tag(std::make_pair(c, c + strlen(c)), allow_extra, sink);
		}

		/**
		 * Tag parsing -- functional version, split tag string.
		 *
		 * This is the main tag parsing function, other variants end up calling
		 * this with some preprocessing and tweaks. Parses the tag string and
		 * calls the sink function with each of the resulting tag objects.
		 *
		 * May throw various tag parse errors.
		 *
		 * The tag string is assumed to be in colon-saprated format, with the
		 * POS name first, followed by attribute values.
		 * The tag string may contain special characters that make the parsing
		 * end up with more than one tag:
		 * - a dot (.) separates several possible values for one field
		 * - an underscore (_) indicates that all values for the attribute at
		 *   the underscore's position should be taken.
		 */
		void parse_tag(const string_range_vector& ts, bool allow_extra, boost::function<void (const Tag&)> sink) const;

		/**
		 * Tag parsing -- plain version, whole string.
		 *
		 * A simple wrapper for string split and a call to the split string version.
		 */
		std::vector<Tag> parse_tag(const string_range& s, bool allow_extra) const;

		/**
		 * Tag parsing -- plain version, whole string, char* overload.
		 *
		 * A simple wrapper for string split and a call to the split string version.
		 */
		std::vector<Tag> parse_tag(const char* c, bool allow_extra) const {
			return parse_tag(std::make_pair(c, c + strlen(c)), allow_extra);
		}

		/**
		 * Tag parsing -- plain version, split string.
		 *
		 * A wrapper to the functional version that sets up parameters so that
		 * the tags end up in a vector, which is then returned.
		 */
		std::vector<Tag> parse_tag(const string_range_vector& ts, bool allow_extra) const;

		/**
		 * Simple tag parsing -- whole string version.
		 *
		 A simple wrapper for string split and a call to the split string version.
		 */
		Tag parse_simple_tag(const string_range& s, bool allow_extra) const;

		/**
		 * Simple tag parsing -- whole string version, char* overload.
		 *
		 A simple wrapper for string split and a call to the split string version.
		 */
		Tag parse_simple_tag(const char* c, bool allow_extra) const {
			return parse_simple_tag(std::make_pair(c, c + strlen(c)), allow_extra);
		}

		/**
		 * Simple tag parsing -- split string version.
		 *
		 * The tag string must not contain any special characters that make the
		 * string result on more than one actual tag object (dot or underscore).
		 */
		Tag parse_simple_tag(const string_range_vector& ts, bool allow_extra) const;


		/**
		 * Create a tag from the given POS and a (unsorted) vector of values.
		 *
		 * The POS is assumed to be valid in this tagset.
		 * The values are assumed to be valid in this tagset, but are checked
		 * for correctness with regards to the POS.
		 */
		Tag make_tag(pos_idx_t pos, const std::vector<value_idx_t>& values, bool allow_extra) const;

		Tag make_ign_tag() const;


		/**
		 * Check if the tag is valid, that is:
		 * * has a valid POS
		 * * all atributes are valid, with either no value or a valid value for
		 *   that attribute
		 * * all required attributes are set
		 * * no extra attrbutes are set, unless allow_extra is true
		 * @return true if the tag is valid, false otherwise
		 */
		bool validate_tag(const Tag& t, bool allow_extra);

		/**
		 * Create the string representation of a tag.
		 *
		 * The output format is the same as the input format of the parsing
		 * functions. If the tag is valid, parsing the resulting string again
		 * should result in the exact same tag object (invalid tags will be
		 * output as strings anyway, but the string may trigger a parse error).
		 */
		std::string tag_to_string(const Tag& tag) const;

		/// POS name <-> index dictionary getter
		const SymbolDictionary<pos_idx_t>& pos_dictionary() const {
			return pos_dict_;
		}

		/// attribute name <-> index dictionary getter
		const SymbolDictionary<attribute_idx_t>& attribute_dictionary() const {
			return attribute_dict_;
		}

		/// value name <-> index dictionary getter
		const SymbolDictionary<value_idx_t>& value_dictionary() const {
			return value_dict_;
		}

		/// Getter for the value -> attribute mapping
		attribute_idx_t get_value_attribute(value_idx_t id) const;

		/// Getter for the attribute -> valid values mapping
		const std::vector<value_idx_t>& get_attribute_values(attribute_idx_t a) const;

		/// Getter for the pos -> valid attributes (in order) mapping
		const std::vector<attribute_idx_t>& get_pos_attributes(pos_idx_t pos) const;

		/// Getter for the pos -> valid attributes flag vector
		const std::vector<bool>& get_pos_valid_attributes(pos_idx_t pos) const;

		/// Getter for the pos -> required attributes flag vector
		const std::vector<bool>& get_pos_required_attributes(pos_idx_t pos) const;

		/**
		 * Tagset cardinality counter -- the number of different valid tags
		 * this tagset represents.
		 */
		size_t size() const;

		/**
		 * Tagset cardinality counter -- the number of all the different tags
		 * this tagset represents, both valid and invalid.
		 *
		 * Return type is double since this can easily become huge.
		 */
		double size_extra() const;

		tagset_idx_t id() const {
			return id_;
		}

	private:
		friend class TagsetParser;

		std::string id_string_;

		tagset_idx_t id_;

		/// String - number dictionary for the POS names
		SymbolDictionary<pos_idx_t> pos_dict_;

		/// String - number dictionary for the attribute names
		SymbolDictionary<attribute_idx_t> attribute_dict_;

		/// String - number dictionary for the attribute values
		SymbolDictionary<value_idx_t> value_dict_;

		/// mapping from attribute indices to valid value indices
		std::vector< std::vector<value_idx_t> > attribute_values_;

		/// reverse mapping, from a value index to the respective attribute index
		/// (values are assumed to be unique and not shared between attributes)
		std::vector<attribute_idx_t> value_attribute_;

		/// POS to valid attribute indices mapping
		/// The order of the attributes is important, as it affects string
		/// output and the behavior of the _ special character in parsing
		std::vector< std::vector<attribute_idx_t> > pos_attributes_;

		/// Flags for attributes which are valid for a given POS
		std::vector< std::vector<bool> > pos_valid_attributes_;

		/// Flags for attributes which are required for a given POS
		std::vector< std::vector<bool> > pos_required_attributes_;

	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TAGSET_H
