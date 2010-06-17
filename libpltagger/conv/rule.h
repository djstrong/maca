#ifndef LIBPLTAGGER_CONV_RULE_H
#define LIBPLTAGGER_CONV_RULE_H

#include <libpltagger/tagset.h>
#include <libpltagger/token.h>
#include <boost/function.hpp>

namespace PlTagger {

	class ConvRule
	{
	public:
		ConvRule();

		virtual ~ConvRule();

		virtual void apply(Tag& tag);

		Tag apply_copy(const Tag& tag);
	};

	class JoinRule : public ConvRule
	{

	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CONV_RULE_H
