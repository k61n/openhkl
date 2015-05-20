#define BOOST_TEST_MODULE "Test Diffractometer Store"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include "DiffractometerStore.h"
#include "Detector.h"
#include "Sample.h"

BOOST_AUTO_TEST_CASE(Test_DiffractometerStore)
{

	SX::Instrument::DiffractometerStore* ds=SX::Instrument::DiffractometerStore::Instance();

	for (const auto& p : ds->getDiffractometersList())
	{
		std::cout<<p<<std::endl;
		auto diffracto = ds->buildDiffractomer(p);
	}

}

