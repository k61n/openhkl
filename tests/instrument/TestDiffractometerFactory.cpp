#define BOOST_TEST_MODULE "Test Diffractometer Factory"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "Detector.h"
#include "Sample.h"
#include "Diffractometer.h"
#include "DiffractometerFactory.h"

using namespace SX::Instrument;

BOOST_AUTO_TEST_CASE(Test_Diffractometer_Factory)
{

	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	Diffractometer* diff = factory->create("D19-4c","my d19 diff");

	BOOST_CHECK_EQUAL(diff->getDetector()->getNCols(),640);
	BOOST_CHECK_EQUAL(diff->getDetector()->getNRows(),256);

	diff = factory->create("D9-4c","my d9 diff");

	BOOST_CHECK_EQUAL(diff->getDetector()->getNCols(),32);
	BOOST_CHECK_EQUAL(diff->getDetector()->getNRows(),32);

}

