#define BOOST_TEST_MODULE "Test MetaData class"
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
#include "HDF5Data.h"
#include "Units.h"

using namespace SX::Data;
using namespace SX::Instrument;
using namespace SX::Units;

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_MetaData)
{
    // todo: implement me!
}
