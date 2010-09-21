#include <libmaca/conv/splitlayer.h>
#include <libcorpus2/tagsetmanager.h>
#include <libpwrutils/foreach.h>
#include <libmaca/conv/attributecopier.h>
#include <boost/algorithm/string.hpp>

namespace Maca {
namespace Conversion {

TwoSplitLayer::TwoSplitLayer(const Tagset& tagset)
	: OneTagsetLayer(tagset)
	, queue_()
	, orth_matcher_(), orth_pattern_()
	, pre_(), t1_post_(), copy_attrs_to_t2_()
{
}

TwoSplitLayer::TwoSplitLayer(const Config::Node& cfg)
	: OneTagsetLayer(get_named_tagset(cfg.get<std::string>("tagset")))
	, queue_()
	, orth_matcher_(NULL) , pre_(), t1_post_(), copy_attrs_to_t2_()
{
	std::string re = cfg.get<std::string>("regexp");
	if (re.empty()) throw ConfigValueMissing("regexp", "split layer");
	set_orth_regexp(re);
	foreach (const Config::Node::value_type &v, cfg) {
		if (v.first == "pre") {
			add_precondition(v.second.data());
		} else if (v.first == "t1_post") {
			add_t1_postcondition(v.second.data());
		} else if (v.first == "copy_attrs_to_t2") {
			append_copy_attrs_to_t2(v.second.data());
		} else if (v.first == "t2_lemma") {
			t2_lexeme_.set_lemma(UnicodeString::fromUTF8(v.second.data()));
		} else if (v.first == "t2_tag") {
			t2_lexeme_.set_tag(
					tagset().parse_simple_tag(v.second.data(), true));
		}
	}
	if (t2_lexeme_.is_null()) {
		throw MacaError("3-split layer: invalid token 2 lexeme");
	}
}

TwoSplitLayer::~TwoSplitLayer()
{
	foreach (const Token* t, queue_) {
		delete t;
	}
}

TwoSplitLayer* TwoSplitLayer::clone() const
{
	TwoSplitLayer* copy = new TwoSplitLayer(tagset());
	clone_helper(copy);
	return copy;
}

void TwoSplitLayer::clone_helper(TwoSplitLayer* copy) const
{
	if (orth_pattern_) {
		copy->orth_pattern_ = orth_pattern_;
		UErrorCode status = U_ZERO_ERROR;
		copy->orth_matcher_.reset(copy->orth_pattern_->matcher(status));
	}
	copy->pre_ = pre_;
	copy->t1_post_ = t1_post_;
	copy->copy_attrs_to_t2_ = copy_attrs_to_t2_;
	copy->t2_lexeme_ = copy->t2_lexeme_;
	foreach (const Token* t, queue_) {
		copy->queue_.push_back(t->clone());
	}
}

void TwoSplitLayer::add_copy_attr_to_t2(attribute_idx_t a)
{
	copy_attrs_to_t2_.push_back(a);
}

void TwoSplitLayer::append_copy_attrs_to_t2(const std::string& s)
{
	append_attribute_list(copy_attrs_to_t2_, tagset(), s);
}

void TwoSplitLayer::add_precondition(const TagPredicate &tp)
{
	pre_.push_back(tp);
}

void TwoSplitLayer::add_precondition(const std::string& pred_string)
{
	std::vector<std::string> srv;
	boost::algorithm::split(srv, pred_string,
			boost::is_any_of(std::string(": ")));
	foreach (const std::string& sr, srv) {
		if (!sr.empty()) {
			pre_.push_back(TagPredicate(sr, tagset()));
		}
	}
}

void TwoSplitLayer::add_t1_postcondition(const TagPredicate &tp)
{
	t1_post_.push_back(tp);
}

void TwoSplitLayer::add_t1_postcondition(const std::string& pred_string)
{
	std::vector<std::string> srv;
	boost::algorithm::split(srv, pred_string,
			boost::is_any_of(std::string(": ")));
	foreach (const std::string& sr, srv) {
		if (!sr.empty()) {
			t1_post_.push_back(TagPredicate(sr, tagset()));
		}
	}
}

void TwoSplitLayer::set_orth_regexp(const std::string &regexp_string)
{
	UErrorCode status = U_ZERO_ERROR;
	UnicodeString u_regexp_string = UnicodeString::fromUTF8(regexp_string);
	orth_pattern_.reset(RegexPattern::compile(u_regexp_string, 0, status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
	orth_matcher_.reset(orth_pattern_->matcher(status));
	if (!U_SUCCESS(status)) {
		throw MacaError("Regexp failed to compile: " + regexp_string);
	}
	if (orth_matcher_->groupCount() < 2) {
		throw MacaError("Split layer regex has less than 2 groups");
	}
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
				Token* t2 = new Token(orth_matcher_->group(2, status),
						Toki::Whitespace::None);
				t2->add_lexeme(t2_lexeme_);
				copy_attributes(*t, copy_attrs_to_t2_, *t2);
				queue_.push_back(t2);
				t->set_orth(orth_matcher_->group(1, status));

				apply_predicates(t1_post_, *t);
			}
		}
	}
	return t;
}

ThreeSplitLayer::ThreeSplitLayer(const Tagset &tagset)
	: TwoSplitLayer(tagset)
{
}

ThreeSplitLayer::ThreeSplitLayer(const Config::Node &cfg)
	: TwoSplitLayer(cfg)
{
	foreach (const Config::Node::value_type &v, cfg) {
		if (v.first == "copy_attrs_to_t3") {
			append_copy_attrs_to_t3(v.second.data());
		} else if (v.first == "t3_lemma") {
			t3_lexeme_.set_lemma(UnicodeString::fromUTF8(v.second.data()));
		} else if (v.first == "t3_tag") {
			t3_lexeme_.set_tag(
					tagset().parse_simple_tag(v.second.data(), true));
		}
	}
	if (orth_matcher_->groupCount() < 3) {
		throw MacaError("3-split layer regex has less than 3 groups");
	}
	if (t3_lexeme_.is_null()) {
		throw MacaError("3-split layer: invalid token 3 lexeme");
	}
}

ThreeSplitLayer* ThreeSplitLayer::clone() const
{
	ThreeSplitLayer* copy = new ThreeSplitLayer(tagset());
	TwoSplitLayer::clone_helper(copy);
	copy->copy_attrs_to_t3_ = copy_attrs_to_t3_;
	copy->t3_lexeme_ = t3_lexeme_;
	return copy;
}

void ThreeSplitLayer::add_copy_attr_to_t3(attribute_idx_t a)
{
	copy_attrs_to_t3_.push_back(a);
}

void ThreeSplitLayer::append_copy_attrs_to_t3(const std::string& s)
{
	append_attribute_list(copy_attrs_to_t3_, tagset(), s);
}

void ThreeSplitLayer::set_t3_lexeme(const Lexeme &lex)
{
	t3_lexeme_ = lex;
}

Token* ThreeSplitLayer::get_next_token()
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
				Token* t2 = new Token(orth_matcher_->group(2, status),
						Toki::Whitespace::None);
				t2->add_lexeme(t2_lexeme_);
				copy_attributes(*t, copy_attrs_to_t2_, *t2);
				queue_.push_back(t2);
				Token* t3 = new Token(orth_matcher_->group(3, status),
						Toki::Whitespace::None);
				t3->add_lexeme(t3_lexeme_);
				copy_attributes(*t, copy_attrs_to_t3_, *t3);
				queue_.push_back(t3);

				t->set_orth(orth_matcher_->group(1, status));
				apply_predicates(t1_post_, *t);
			}
		}
	}
	return t;
}

} /* end ns Conversion */
} /* end ns Maca */
