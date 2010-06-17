#include <libpltagger/conv/rule.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	ConvRule::ConvRule()
	{
	}

	ConvRule::~ConvRule()
	{
	}

	Tag& ConvRule::apply_copy(const Tag &tag)
	{
		Tag tag2(tag);
		apply(tag2);
		return tag2;
	}

} /* end ns PlTagger */
