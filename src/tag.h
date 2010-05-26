#ifndef LIBPLTAGGER_TAG_H
#define LIBPLTAGGER_TAG_H

#include <string>

namespace PlTagger {

	class Tag
	{
	public:
		Tag();

		explicit Tag(const std::string& str);

		std::string to_string() const;

	private:
		std::string str_;
	};

} /* end ns PlTagger */


#endif // LIBPLTAGGER_TAG_H
