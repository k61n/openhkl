#define BOOST_TEST_MODULE "Test Cyclop"
#define BOOST_TEST_DYN_LINK
#include "Cyclops.h"
#include <boost/test/unit_test.hpp>
#include "DiffractometerFactory.h"
#include "Detector.h"


BOOST_AUTO_TEST_CASE(Test_Cyclop)
{
	auto factory=SX::Instrument::DiffractometerFactory::Instance();
	SX::Instrument::Diffractometer* cyclop=factory->create("Cyclops","");
	double g,nu;
	for (int i=0;i<7680;++i)
	{
		for (int j=0;j<2400;++j)
			cyclop->getDetector()->getGammaNu(i,j,g,nu);
	}
}
