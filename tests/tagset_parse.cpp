#include <boost/test/unit_test.hpp>
#include <set>
#include <libtoki/foreach.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/tagsetmanager.h>
#include <iostream>

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

BOOST_AUTO_TEST_CASE( size1 )
{
	PlTagger::Tagset t = parse(PRE POSA);
	BOOST_CHECK_EQUAL(t.size(), 1);
	BOOST_CHECK_EQUAL(t.size_extra(), 1);
}

BOOST_AUTO_TEST_CASE( size2 )
{
	PlTagger::Tagset t = parse(PRE POSA " POS2\n");
	BOOST_CHECK_EQUAL(t.size(), 2);
	BOOST_CHECK_EQUAL(t.size_extra(), 2);
}

BOOST_AUTO_TEST_CASE( size3 )
{
	PlTagger::Tagset t = parse(PRE "A a b\n" POSA "POS2 A");
	BOOST_CHECK_EQUAL(t.size(), 3);
	BOOST_CHECK_EQUAL(t.size_extra(), 6);
}

BOOST_AUTO_TEST_CASE( size6 )
{
	PlTagger::Tagset t = parse(PRE "A a b \n B c d\n" POSA "POS2 A\n POS3 [A]\n");
	BOOST_CHECK_EQUAL(t.size(), 6);
	BOOST_CHECK_EQUAL(t.size_extra(), 27);
}

BOOST_AUTO_TEST_CASE( load_named )
{
	BOOST_CHECK_NO_THROW(
		try {
			PlTagger::get_named_tagset("test");
		}catch(PlTagger::PlTaggerError& e) {
			std::cerr << e.info();
			throw;
		}
	);
	BOOST_CHECK_THROW(
		PlTagger::get_named_tagset("__nonexistant_9867s8t"),
		PlTagger::FileNotFound);
}

BOOST_AUTO_TEST_SUITE_END();
