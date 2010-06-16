#include <boost/test/unit_test.hpp>

#include <libpltagger/token.h>

BOOST_AUTO_TEST_CASE( token )
{
	PlTagger::Token t(UnicodeString::fromUTF8("ZZ"), Toki::Whitespace::ManySpaces);
	BOOST_CHECK_EQUAL(t.orth_utf8(), "ZZ");
	BOOST_CHECK_EQUAL(t.wa(), Toki::Whitespace::ManySpaces);
	BOOST_CHECK(t.lexemes().empty());
}
