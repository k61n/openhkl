#define BOOST_TEST_MODULE TestRotAxis
#define BOOST_TEST_DYN_LINK
#include "RotAxis.h"
#include <boost/test/unit_test.hpp>

int add( int i, int j ) { return i+j; }

BOOST_AUTO_TEST_CASE( my_test )
{
    SX::Geometry::RotAxis a;
}
