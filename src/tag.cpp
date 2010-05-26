#include "tag.h"

namespace PlTagger {

	Tag::Tag()
	{
	}

	Tag::Tag(const std::string &str)
		: str_(str)
	{
	}

	std::string Tag::to_string() const
	{
		return str_;
	}

} /* end ns PlTagger */
