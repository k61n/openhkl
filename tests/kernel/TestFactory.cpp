#define BOOST_TEST_MODULE "Test Factory"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <string>

#include "Factory.h"

using namespace SX::Kernel;

BOOST_AUTO_TEST_CASE(Test_Factory)
{

	struct A
	{
		static A* create()
		{
			return new A;
		}

	};

	struct AFactory : public Factory<A,std::string>
	{
	};

	AFactory fact=AFactory();
	fact.registerCallback("a",&A::create);

}
