#include <boost/test/unit_test.hpp>
#include <set>
#include <libtoki/foreach.h>
#include "tagsetparser.h"

BOOST_AUTO_TEST_SUITE( tagset_parse );

PlTagger::Tagset parse(const char* s)
{
	std::stringstream ss;
	ss << s;
	return PlTagger::TagsetParser::load_ini(ss);
}

#define PRE "[ATTR]\n"
#define POSA "[POS]\n POS1\n"

BOOST_AUTO_TEST_CASE( empty )
{
	BOOST_CHECK_THROW(
		parse(""), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( minimal )
{
	try {
		parse(PRE POSA);
	} catch (PlTagger::TagsetParseError& e) {
		BOOST_FAIL(e.info());
	}
}
BOOST_AUTO_TEST_CASE( minimal_nonewline )
{
	try {
		parse(PRE "[POS]\n POS1");
	} catch (PlTagger::TagsetParseError& e) {
		BOOST_FAIL(e.info());
	}
}

BOOST_AUTO_TEST_CASE( dupe_val )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a a " POSA), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_val2 )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n C x a" POSA), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_sym )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n a x z" POSA), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_sym2 )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d" POSA "A B"), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_attr )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\nB c d\n C x z" POSA), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( dupe_pos )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\n" POSA "P1 A\n P1\n"), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( bad_pos_attr )
{
	BOOST_CHECK_THROW(
		parse(PRE "A a b\n" POSA "P1 A\n P2 ZZ\n"), PlTagger::TagsetParseError
	);
}

BOOST_AUTO_TEST_CASE( bad_value_name )
{
	BOOST_CHECK_THROW(
		parse(PRE "@@ a b\n" POSA "P1 A\n P2"), PlTagger::TagsetParseError
	);
}
BOOST_AUTO_TEST_SUITE_END();
