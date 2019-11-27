#include "test/cpp/catch.hpp"
#include "base/utils/Random.h"
#include <iostream>


TEST_CASE(__FILE__, "")
{
	try
	{
		double d1 = nsx::Random::double01();
		double d2 = nsx::Random::double01();

		double d3 = nsx::Random::doubleRange(0, 1000);
		double d4 = nsx::Random::doubleRange(0, 1000);

		CHECK(d1 != d2);
		CHECK(d3 != d4);


		int i1 = nsx::Random::intRange();
		int i2 = nsx::Random::intRange();

		CHECK(i1 != i2);
	}
	catch (std::exception& e)
	{
		FAIL(std::string("caught exception: ") + e.what());
	} catch (...)
	{
		FAIL("unknown exception while loading data");
	}
}
