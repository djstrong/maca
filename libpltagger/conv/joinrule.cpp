#include <libpltagger/conv/joinrule.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	JoinRule::JoinRule(const Tagset& tagset)
		: tagset_(&tagset)
	{
	}

	JoinRule::~JoinRule()
	{
	}

	void JoinRule::set_token1_preconditions(const PosOrthPredicate &pre)
	{
		pre1_ = pre;
	}

	void JoinRule::set_token1_preconditions(const std::string& pos, const UnicodeString& orth)
	{
		pos_idx_t p = tagset_->pos_dictionary().get_id(pos);
		if (!tagset_->pos_dictionary().is_id_valid(p)) {
			p = static_cast<pos_idx_t>(-1);
		}
		std::cerr << pos << static_cast<int>(p);
		pre1_ = PosOrthPredicate(p, orth);
	}

	void JoinRule::set_token2_preconditions(const PosOrthPredicate &pre)
	{
		pre1_ = pre;
	}

	void JoinRule::set_token2_preconditions(const std::string& pos, const UnicodeString& orth)
	{
		pos_idx_t p = tagset_->pos_dictionary().get_id(pos);
		if (!tagset_->pos_dictionary().is_id_valid(p)) {
			p = static_cast<pos_idx_t>(-1);
		}
		std::cerr << pos << static_cast<int>(p);
		pre2_ = PosOrthPredicate(p, orth);
	}

	void JoinRule::set_copy_attrs(const std::vector<attribute_idx_t> &v)
	{
		copy_t2_attrs_ = v;
	}

	void JoinRule::add_copy_attr(const std::string& name)
	{
		attribute_idx_t a = tagset_->attribute_dictionary().get_id(name);
		copy_t2_attrs_.push_back(a);
	}

	void JoinRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	void JoinRule::add_postcondition(const std::string& pred_string)
	{
		post_.push_back(TagPredicate(pred_string, *tagset_));
	}

	Token* JoinRule::try_join(Token* t1, Token* t2) const
	{
		if (pre1_.check(*t1) && pre2_.check(*t2)) {
			t1->set_orth(t1->orth() + t2->orth());
			std::vector<Lexeme> new_lexemes;
			foreach (const Lexeme& lex1, t1->lexemes()) {
				foreach (const Lexeme& lex2, t2->lexemes()) {
					Lexeme lex = lex1;
					std::cerr << lex.tag().raw_dump() << "\n";
					std::cerr << lex2.tag().raw_dump() << "\n";
					foreach (attribute_idx_t a, copy_t2_attrs_) {
						lex.tag().values()[a] = lex2.tag().values()[a];
					}
					std::cerr << lex.tag().raw_dump() << "\n";
					foreach (const TagPredicate& p, post_) {
						p.apply(lex.tag());
					}
					std::cerr << lex.tag().raw_dump() << "\n";
					new_lexemes.push_back(lex);
				}
			}
			t1->lexemes() = new_lexemes;
			delete t2;
			return t1;
		} else {
			return NULL;
		}
	}


} /* end ns Conversion */ } /* end ns PlTagger */
