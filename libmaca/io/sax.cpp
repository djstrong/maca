#include <libmaca/io/sax.h>

namespace Maca {

	BasicSaxParser::BasicSaxParser()
		: xmlpp::SaxParser(), buf_()
	{
	}

	void BasicSaxParser::on_characters(const Glib::ustring &text)
	{
		buf_ << static_cast<std::string>(text);
	}

	void BasicSaxParser::on_error(const Glib::ustring &text)
	{
		std::cerr << "XML Error: " << (std::string)text << "\n";
	}
	void BasicSaxParser::on_fatal_error(const Glib::ustring &text)
	{
		std::cerr << "XML Fatal error: " << (std::string)text << "\n";
	}

} /* end ns Maca */
