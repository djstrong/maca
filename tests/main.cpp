//#define BOOST_TEST_MODULE master
#include <boost/test/included/unit_test.hpp>
#include "compareconv.h"

BOOST_AUTO_TEST_CASE(test_test)
{
	int a = 0;
	BOOST_CHECK(a == 0);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
	boost::unit_test::test_suite* ts1 = BOOST_TEST_SUITE("compare");
	std::string compare_path;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--compare-tests-dir") == 0) {
			++i;
			if (i < argc) {
				compare_path = argv[i];
			}
		}
	}
	init_compareconv_suite(ts1, compare_path);
	boost::unit_test::framework::master_test_suite().add(ts1);
	return 0;
}
