#ifndef LIBPLTAGGER_TOKEN_H
#define LIBPLTAGGER_TOKEN_H

#include "lexeme.h"

#include <unicode/unistr.h>
#include <string>
#include <libtoki/whitespace.h>
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
