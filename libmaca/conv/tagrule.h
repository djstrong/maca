#ifndef LIBMACA_CONV_TAGRULE_H
#define LIBMACA_CONV_TAGRULE_H

#include <libcorpus2/tagset.h>
#include <libcorpus2/token.h>
#include <libmaca/conv/predicate.h>
#include <boost/function.hpp>

namespace Maca {
namespace Conversion {

/**
 * A tag rule that checks if all of its preconditions match, and if so,
 * applies all the postcondition predicates.
 */
class TagRule
{
public:
	/**
	 * Create a rule that operates within a tagset
	 */
	TagRule(const Corpus2::Tagset& tagset);

	/**
	 * Add a precondition predicate
	 */
	void add_precondition(const TagPredicate& tp);

	/**
	 * Add a precondition predicate cratead from a string name, see
	 * TagPredicate string constructor.
	 */
	void add_precondition(const std::string& pred_string);

	/**
	 * Add a postcondition predicate
	 */
	void add_postcondition(const TagPredicate& tp);

	/**
	 * Add a postcondition predicate cratead from a string name, see
	 * TagPredicate string constructor.
	 */
	void add_postcondition(const std::string& pred_string);

	/**
	 * Apply the rule on a tag -- check preconditions and if all match,
	 * apply the postconditions.
	 */
	void apply(Corpus2::Tag& tag) const;

	/**
	 * Apply the rule on a tag -- check preconditions and if all match,
	 * apply the postconditions, tag copying version.
	 */
	Corpus2::Tag apply_copy(const Corpus2::Tag& tag) const;

private:
	/// The tagset, used for string condition adders
	const Corpus2::Tagset* tagset_;

	/// The precondition predicates
	std::vector<TagPredicate> pre_;

	/// The postcondition predicates
	std::vector<TagPredicate> post_;
};

} /* end ns Conversion */
} /* end ns Maca */

#endif // LIBMACA_CONV_TAGRULE_H
