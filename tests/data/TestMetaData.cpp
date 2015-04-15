#define BOOST_TEST_MODULE "Test Ascii format"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "MetaData.h"

using namespace SX::Data;

BOOST_AUTO_TEST_CASE(Test_Ascii_Reader)
{

	// Make sure that total counts for the first frame is 65.
	MetaData* meta= new MetaData();

	delete meta;

}
