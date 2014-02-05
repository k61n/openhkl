#define BOOST_TEST_MODULE "Test reading MetaData in ILL AScii Format"
#define BOOST_TEST_DYN_LINK
#include "ILLAsciiMetaReader.h"
#include "MetaData.h"
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <string>

using namespace SX::Data;

BOOST_AUTO_TEST_CASE(Test_ILLAscii_Meta_Reader)
{
	//Empty Rotation axis initialize to (0,0,1)
	ILLAsciiMetaReader* instance=ILLAsciiMetaReader::Instance();
	std::size_t sof;
	MetaData* m=instance->read(std::string("D10_ascii_example"),sof);

	// MetaData block should be 42 lines long
	BOOST_CHECK(sof==81*42);

	// Check that 2 angles are scanned
	BOOST_CHECK(m->getKey<int>("nbang")==2);

}
