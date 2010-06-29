#include <libpltagger/conv/splitlayer.h>
#include <libpltagger/tagsetmanager.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	TwoSplitLayer::TwoSplitLayer(const Tagset& tagset)
		: OneTagsetLayer(tagset)
		, queue_()
		, orth_matcher_(NULL) , pre_(), t1_post_(), copy_attrs_to_t2_()
	{
	}

	TwoSplitLayer::TwoSplitLayer(const Config::Node& cfg)
		: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset")))
		, queue_()
		, orth_matcher_(NULL) , pre_(), t1_post_(), copy_attrs_to_t2_()
	{
		std::string re = cfg.get<std::string>("regexp");
		if (re.empty()) throw 9;
		set_orth_regexp(re);
		foreach (const Config::Node::value_type &v, cfg) {
			if (v.first == "pre") {
				add_precondition(v.second.data());
			} else if (v.first == "t1_post") {
				add_t1_postcondition(v.second.data());
			} else if (v.first == "copy_attrs_to_t2") {
				add_copy_attr(v.second.data());
			}
		}
	}

	TwoSplitLayer::~TwoSplitLayer()
	{
		delete orth_matcher_;
	}

	void TwoSplitLayer::add_copy_attr(attribute_idx_t a)
	{
		copy_attrs_to_t2_.push_back(a);
	}

	void TwoSplitLayer::add_copy_attr(const std::string& s)
	{
		attribute_idx_t a = tagset_from().attribute_dictionary().get_id(s);
		if (tagset_from().attribute_dictionary().is_id_valid(a)) {
			copy_attrs_to_t2_.push_back(a);
		}
	}

	void TwoSplitLayer::add_precondition(const TagPredicate &tp)
	{
		pre_.push_back(tp);
	}

	void TwoSplitLayer::add_precondition(const std::string& pred_string)
	{
		TagPredicate tp(pred_string, tagset_from());
		pre_.push_back(tp);
	}

	void TwoSplitLayer::add_t1_postcondition(const TagPredicate &tp)
	{
		t1_post_.push_back(tp);
	}

	void TwoSplitLayer::add_t1_postcondition(const std::string& pred_string)
	{
		TagPredicate tp(pred_string, tagset_from());
		t1_post_.push_back(tp);
	}

	void TwoSplitLayer::set_orth_regexp(const std::string &regexp_string)
	{
		UErrorCode status = U_ZERO_ERROR;
		orth_matcher_ = new RegexMatcher(UnicodeString::fromUTF8(regexp_string), 0, status);
		if (!U_SUCCESS(status)) throw 9;
	}

	void TwoSplitLayer::set_t2_lexeme(const Lexeme &lex)
	{
		t2_lexeme_ = lex;
	}

	Token* TwoSplitLayer::get_next_token()
	{
		Token* t;
		if (!queue_.empty()) {
			t = queue_.front();
			queue_.pop_front();
		} else {
			t = source()->get_next_token();
			if (t != NULL) {
				foreach (const TagPredicate& tp, pre_) {
					if (!tp.token_match(*t)) return t;
				}
				orth_matcher_->reset(t->orth());
				UErrorCode status = U_ZERO_ERROR;
				if (orth_matcher_->matches(status)) {
					Token* t2 = new Token(orth_matcher_->group(2, status), Toki::Whitespace::None);
					t2->add_lexeme(t2_lexeme_);
					foreach (attribute_idx_t a, copy_attrs_to_t2_) {
						t2->lexemes()[0].tag().values()[a] = t->lexemes()[0].tag().values()[a];
					}
					queue_.push_back(t2);
					t->set_orth(orth_matcher_->group(1, status));
					foreach (const TagPredicate& tp, t1_post_) {
						tp.token_apply(*t);
					}
				}
			}
		}
		return t;
	}

} /* end ns Conversion */ } /* end ns PlTagger */
