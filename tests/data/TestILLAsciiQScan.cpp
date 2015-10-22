#define BOOST_TEST_MODULE "Test ILL Ascii QScan"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include "ComponentState.h"
#include "DiffractometerStore.h"
#include "ILLAsciiData.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_ILL_Ascii_QScan)
{

	DiffractometerStore* ds = DiffractometerStore::Instance();

	std::shared_ptr<Diffractometer> diff = std::shared_ptr<Diffractometer>(ds->buildDiffractomer("D9"));

	ILLAsciiData dataf(std::string("D9_QSCAN"),diff);

	MetaData* meta=dataf.getMetadata();

	BOOST_CHECK(meta->getKey<int>("nbang")==4);

	dataf.open();

	Eigen::MatrixXi v=dataf.getFrame(0);

	// Check the total number of count in the frame 0
	BOOST_CHECK_EQUAL(v.sum(),5.90800000e+03);

	// Check the value of the monitor
	BOOST_CHECK_CLOSE(meta->getKey<double>("monitor"),3.74130000e+04,tolerance);



	dataf.close();

}
