#define BOOST_TEST_MODULE "Test whether h,k,l peak falls into Data"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "ComponentState.h"
#include "DiffractometerFactory.h"
#include "ILLAsciiData.h"
#include "HDF5Data.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_Peak_Data)
{
	DiffractometerFactory* factory = DiffractometerFactory::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(factory->create("D9 4-circles","D9 diffractometer")->clone());

	ILLAsciiData dataf("D9_ascii_example",diff);

	dataf.open();
	dataf.readInMemory();

	auto metadata=dataf.getMetadata();
	double ub11=metadata->getKey<double>("ub(1,1)");
	double ub12=metadata->getKey<double>("ub(1,2)");
	double ub13=metadata->getKey<double>("ub(1,3)");
	double ub21=metadata->getKey<double>("ub(2,1)");
	double ub22=metadata->getKey<double>("ub(2,2)");
	double ub23=metadata->getKey<double>("ub(2,3)");
	double ub31=metadata->getKey<double>("ub(3,1)");
	double ub32=metadata->getKey<double>("ub(3,2)");
	double ub33=metadata->getKey<double>("ub(3,3)");

	Eigen::Matrix3d ub;
	ub <<   ub11,ub12,ub13,
			ub21,ub22,ub23,
			ub31,ub32,ub33;
	ub.transposeInPlace();

}
