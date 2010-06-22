#include <libpltagger/conv/joinrule.h>
#include <libtoki/foreach.h>

namespace PlTagger { namespace Conversion {

	JoinRule::JoinRule()
	{
	}

	JoinRule::~JoinRule()
	{
	}

	void JoinRule::set_preconditions(const PosOrthPredicate &pre1, const PosOrthPredicate &pre2)
	{
		pre1_ = pre1;
		pre2_ = pre2;
	}

	void JoinRule::set_copy_attrs(const std::vector<attribute_idx_t> &v)
	{
		copy_t2_attrs_ = v;
	}

	void JoinRule::add_postcondition(const TagPredicate &tp)
	{
		post_.push_back(tp);
	}

	Token* JoinRule::try_join(Token* t1, Token* t2) const
	{
		if (pre1_.check(*t1) && pre2_.check(*t2)) {
			t1->set_orth(t1->orth() + t2->orth());
			std::vector<Lexeme> new_lexemes;
			foreach (const Lexeme& lex1, t1->lexemes()) {
				foreach (const Lexeme& lex2, t2->lexemes()) {
					Lexeme lex = lex1;
					foreach (attribute_idx_t a, copy_t2_attrs_) {
						lex.tag().values()[a] = lex2.tag().values()[a];
					}
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
