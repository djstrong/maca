#include <boost/test/unit_test.hpp>
#include <set>

BOOST_AUTO_TEST_SUITE( tag_split )

void check_split(const char* orig, const std::set<std::string> expect)
{
	std::set<std::string> actual;
	//
	BOOST_CHECK_EQUAL_COLLECTIONS(actual.begin(), actual.end(), expect.begin(), expect.end());
}


BOOST_AUTO_TEST_CASE( plain )
{
	const char tag[] = "some:tag:data";
	std::set<std::string> r;
	r.insert(tag);
	check_split(tag, r);
}

BOOST_AUTO_TEST_CASE( plus )
{
	const char tag[] = "some:tag:data+some:other:tag";
	std::set<std::string> result;
	result.insert("some:tag:data");
	result.insert("some:other:tag");
	check_split(tag, result);
}

BOOST_AUTO_TEST_CASE( dot )
{
	const char tag[] = "some:tag.tog:data";
	std::set<std::string> result;
	result.insert("some:tag:data");
	result.insert("some:tog:tag");
	check_split(tag, result);
}

BOOST_AUTO_TEST_CASE( dots )
{
	const char tag[] = "some:tag.tog:data:a.b.c";
	std::set<std::string> result;
	result.insert("some:tag:data:a");
	result.insert("some:tog:data:a");
	result.insert("some:tag:data:b");
	result.insert("some:tog:data:b");
	result.insert("some:tag:data:c");
	result.insert("some:tog:data:c");
	check_split(tag, result);
}

BOOST_AUTO_TEST_CASE( dots_plus )
{
	const char tag[] = "some:tag.tog:data:a.b+some:other:thing.thang";
	std::set<std::string> result;
	result.insert("some:tag:data:a");
	result.insert("some:tog:data:a");
	result.insert("some:tag:data:b");
	result.insert("some:tog:data:b");
	result.insert("some:other:thing");
	result.insert("some:other:thang");
	check_split(tag, result);
}


BOOST_AUTO_TEST_SUITE_END()
