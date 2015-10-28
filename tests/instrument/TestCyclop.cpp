#define BOOST_TEST_MODULE "Test Cyclop"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "DiffractometerStore.h"
#include "Detector.h"


BOOST_AUTO_TEST_CASE(Test_Cyclop)
{
	SX::Instrument::DiffractometerStore* ds=SX::Instrument::DiffractometerStore::Instance();
	SX::Instrument::sptrDiffractometer cyclop=ds->buildDiffractomer("Cyclops");
	double g,nu;
	for (int i=0;i<7680;++i)
	{
		for (int j=0;j<2400;++j)
			cyclop->getDetector()->getGammaNu(i,j,g,nu);
	}
}
