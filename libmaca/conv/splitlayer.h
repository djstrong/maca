#ifndef LIBMACA_CONV_SPLITLAYER_H
#define LIBMACA_CONV_SPLITLAYER_H

#include <libmaca/conv/layer.h>
#include <libmaca/conv/predicate.h>
#include <libmaca/util/confignode.h>
#include <deque>
#include <unicode/regex.h>

namespace Maca { namespace Conversion {

	/**
	 * A layer to split tokens matching specific criteria.
	 *
	 * Some precondition predicates are first checked, then a regular expression
	 * is matched on the token's orth. If the regexp matches, tokens will be
	 * created with orths corresponding to the capturing groups (in this case,
	 * the regexp should have exactly two capturing groups). The resulting
	 * tokens are referred to as token 1 and token 2.
	 *
	 * Token 1 retains the lexemes of the original token. Token 2 is created
	 * with a pre-set lexeme, with some attributes possibly copied from the
	 * original token's lexemes.
	 *
	 * Finally, some postcondition predicates are appliked on output token 1.
	 */
	class TwoSplitLayer : public OneTagsetLayer
	{
	public:
		/// Constructor for an empty TwoSplitLayer working within a tagset
		TwoSplitLayer(const Tagset& tagset);

		/**
		 * Config node constructor. Recognized keys are:
		 * - regexp - the regular expression used for orth spliiting, should
		 *            have exactly two capturing groups.
		 * - pre - precondition predicates, can appear multiple times
		 * - t1_post - postconditions predicates of token 1, can appear multiple
		 *             times
		 * - copy_attrs_to_t2 - attributes to copy to token 2 from the original
		 *                      token's lexemes, can appear multiple times
		 * - t2_lemma - lemma of the token 2 lexeme
		 * - t2_tag - tag of the token 2 lexeme
		 */
		TwoSplitLayer(const Config::Node& cfg);

		/// Destructor
		~TwoSplitLayer();

		void set_orth_regexp(const std::string& regexp_string);

		void add_precondition(const TagPredicate& tp);

		void add_precondition(const std::string& pred_string);

		void add_t1_postcondition(const TagPredicate& tp);

		void add_t1_postcondition(const std::string& pred_string);

		void add_copy_attr_to_t2(attribute_idx_t a);

		void append_copy_attrs_to_t2(const std::string& a);

		void set_t2_lexeme(const Lexeme& lex);

		Token* get_next_token();

	protected:
		std::deque<Token*> queue_;

		RegexMatcher* orth_matcher_;

		std::vector<TagPredicate> pre_;

		std::vector<TagPredicate> t1_post_;

		std::vector<attribute_idx_t> copy_attrs_to_t2_;

		Lexeme t2_lexeme_;
	};

	/**
	 * Three-token variant split layer. Main difference from TwoSplitLayer
	 * is that the regexp should have three capturing groups, otherwise this
	 * behaves similarily. The resulting split tokens are referred to as tokens
	 * 1, 2 and 3.
	 */
	class ThreeSplitLayer : public TwoSplitLayer
	{
	public:
		/// Constructor for an empty ThreeSplitLayer working within a tagset
		ThreeSplitLayer(const Tagset& tagset);

		/**
		 * Config node constructor. Recognized keys are:
		 * - copy_attrs_to_t3 - attributes to copy to token 3 from the original
		 *                      token's lexemes, can appear multiple times
		 * - t3_lemma - lemma of the token 3 lexeme
		 * - t3_tag - tag of the token 3 lexeme
		 */
		ThreeSplitLayer(const Config::Node& cfg);

		void add_copy_attr_to_t3(attribute_idx_t a);

		void append_copy_attrs_to_t3(const std::string& a);

		void set_t3_lexeme(const Lexeme& lex);

		Token* get_next_token();

	protected:
		std::vector<attribute_idx_t> copy_attrs_to_t3_;

		Lexeme t3_lexeme_;
	};

} /* end ns Conversion */ } /* end ns Maca */

#endif // LIBMACA_CONV_SPLITLAYER_H