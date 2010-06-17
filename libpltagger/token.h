#ifndef LIBPLTAGGER_TOKEN_H
#define LIBPLTAGGER_TOKEN_H

#include <libpltagger/lexeme.h>
#include <libpltagger/tagset.h>

#include <unicode/unistr.h>
#include <string>
#include <libtoki/token.h>
#include <libtoki/util.h>
#include <vector>

namespace PlTagger {

	/// A single token being processed by the tagger.
	/**
	 * The orth is stored as an ICU UnicodeString.
	 */
	class Token
	{
	public:
		/// Creates an empty Token
		Token();

		/// Create a Token with the given orth and whitespace amount
		Token(const UnicodeString& orth, Toki::Whitespace::Enum wa);

		/// Toki::Token conversion
		explicit Token(const Toki::Token& tok);

		/// Orth getter
		const UnicodeString& orth() const {
			return orth_;
		}

		/// Orth getter (UTF-8)
		std::string orth_utf8() const {
			return Toki::Util::to_utf8(orth_);
		}

		/// Orth setter
		void set_orth(const UnicodeString& orth) {
			orth_ = orth;
		}

		/// WA getter
		const Toki::Whitespace::Enum& wa() const {
			return wa_;
		}

		/// WA setter
		void set_wa(const Toki::Whitespace::Enum& wa) {
			wa_ = wa;
		}

		/// Lexemes getter
		const std::vector<Lexeme>& lexemes() const {
			return lexemes_;
		}

		/// Lexemes setter
		std::vector<Lexeme>& lexemes() {
			return lexemes_;
		}

		/// Lexemes adder
		void add_lexeme(const Lexeme& lex) {
			lexemes_.push_back(lex);
		}

		void make_ign(const Tagset& tagset);

		void add_ign(const Tagset& tagset);

		bool operator==(const Token& other) const;

		/**
		 * Check for duplicate lexemes.
		 *
		 * Returns true if there are duplicate lexemes, false otherwise.
		 */
		bool check_duplicate_lexemes() const;

		/**
		 * Remove duplicate lexemes.
		 *
		 * Removes any duplicate lexemes and returns true if there were any.
		 * Ordering of the lexemes can change.
		 */
		bool remove_duplicate_lexemes();

		/**
		 * Check if all the lexemes of the token have the given POS, and and the
		 * token orth caselessly matches the given orth.
		 *
		 * If the given orth is empty, it is not checked.
		 * If the given pos idx is -1, it is not checked.
		 *
		 * @returns true if the orth and lexemes pass the check, false otherwise
		 */
		bool orth_pos_match(pos_idx_t pos, const UnicodeString& orth);

	private:
		/// The orth (actual encountered form)
		UnicodeString orth_;

		/// The preceeding whitespace amount marker
		Toki::Whitespace::Enum wa_;

		/// The possible lexemes
		std::vector<Lexeme> lexemes_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_TOKEN_H
