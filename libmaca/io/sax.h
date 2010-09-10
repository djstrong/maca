#ifndef LIBMACA_IO_SAX_H
#define LIBMACA_IO_SAX_H

#include <libmaca/io/premorph.h>
#include <libmaca/io/xcescommon.h>
#include <libmaca/util/tokentimer.h>
#include <libtoki/util/foreach.h>
#include <libxml++/parsers/saxparser.h>
#include <libtoki/sentencesplitter.h>
#include <vector>

namespace Maca {

	class BasicSaxParser : public xmlpp::SaxParser
	{
	public:
		BasicSaxParser();

	protected:
		void on_characters(const Glib::ustring & text);
		void on_error(const Glib::ustring &text);
		void on_fatal_error(const Glib::ustring &text);

		std::stringstream buf_;
	};
} /* end ns Maca */

#endif // LIBMACA_IO_SAX_H
