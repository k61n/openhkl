#define BOOST_TEST_MODULE "Test Factory"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <string>
#include <memory>

#include "Factory.h"

using namespace SX::Kernel;
using std::unique_ptr;

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

    unique_ptr<A> ptr = unique_ptr<A>(fact.create("a"));
}
