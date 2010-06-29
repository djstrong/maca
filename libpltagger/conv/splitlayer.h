#ifndef LIBPLTAGGER_CONV_SPLITLAYER_H
#define LIBPLTAGGER_CONV_SPLITLAYER_H

#include <libpltagger/conv/layer.h>
#include <libpltagger/conv/predicate.h>
#include <libpltagger/confignode.h>
#include <deque>
#include <unicode/regex.h>

namespace PlTagger { namespace Conversion {

	class TwoSplitLayer : public OneTagsetLayer
	{
	public:
		TwoSplitLayer(const Tagset& tagset);

		TwoSplitLayer(const Config::Node& cfg);

		~TwoSplitLayer();

		void set_orth_regexp(const std::string& regexp_string);

		void add_precondition(const TagPredicate& tp);

		void add_precondition(const std::string& pred_string);

		void add_t1_postcondition(const TagPredicate& tp);

		void add_t1_postcondition(const std::string& pred_string);

		void add_copy_attr_to_t2(attribute_idx_t a);

		void add_copy_attr_to_t2(const std::string& a);

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

	class ThreeSplitLayer : public TwoSplitLayer
	{
	public:
		ThreeSplitLayer(const Tagset& tagset);

		ThreeSplitLayer(const Config::Node& cfg);

		void add_copy_attr_to_t3(attribute_idx_t a);

		void add_copy_attr_to_t3(const std::string& a);

		void set_t3_lexeme(const Lexeme& lex);

		Token* get_next_token();

	protected:
		std::vector<attribute_idx_t> copy_attrs_to_t3_;

		Lexeme t3_lexeme_;
	};

} /* end ns Conversion */ } /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_SPLITLAYER_H
