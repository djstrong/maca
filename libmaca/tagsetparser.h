#ifndef LIBMACA_TAGSETPARSER_H
#define LIBMACA_TAGSETPARSER_H

#include <libmaca/tagset.h>

namespace Maca {

	class TagsetParseError : public MacaError
	{
	public:
		TagsetParseError(const std::string& w, int line,
				const std::string& data)
			: MacaError("Tagset parse error: " + w), line(line), data(data)
		{
		}

		~TagsetParseError() throw()
		{
		}

		virtual std::string info() const;

		int line;
		std::string data;
	};

	class TagsetParser
	{
	public:
		static Tagset load_ini(const std::string& filename);

		static Tagset load_ini(std::istream& is);

		static void save_ini(const Tagset& tagset,
				const std::string& filename);

		static void save_ini(const Tagset& tagset, std::ostream& os);

	};

} /* end ns Maca */

#endif // LIBMACA_TAGSETPARSER_H
