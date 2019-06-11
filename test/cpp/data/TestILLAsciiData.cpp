#include "test/cpp/catch.hpp"
#include <fstream>
#include <string>

#include <Eigen/Dense>

#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/algo/DataReaderFactory.h"
#include "core/loader/IDataReader.h"
#include "core/loader/MetaData.h"
#include "base/utils/Units.h"

const double tolerance = 1e-2;

TEST_CASE("test/data/TestILLAsciiData.cpp", "")
{
    nsx::DataReaderFactory factory;
    nsx::Diffractometer* diffractometer;
    nsx::sptrDataSet dataf;
    Eigen::MatrixXi v;

    nsx::MetaData metadata;

    try {
        diffractometer = nsx::Diffractometer::create("D10");

        dataf = factory.create("", "D10_ascii_example", diffractometer);

        metadata = dataf->reader()->metadata();

        CHECK(metadata.key<int>("nbang") == 2);

        dataf->open();
        v = dataf->frame(0);
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception while loading data");
    }

    // Check the total number of count in the frame 0
    CHECK(v.sum() == 65);

    // Check the value of the monitor
    CHECK(metadata.key<double>("monitor") == Approx(20000).epsilon(tolerance));

    auto sampleStates = dataf->reader()->sampleStates();
    auto detectorStates = dataf->reader()->detectorStates();

    CHECK(detectorStates[3][0] == Approx(0.54347000E+05 / 1000.0 * nsx::deg).epsilon(tolerance));
    CHECK(sampleStates[2][0] == Approx(0.26572000E+05 / 1000.0 * nsx::deg).epsilon(tolerance));
    CHECK(sampleStates[2][1] == Approx(0.48923233E+02 * nsx::deg).epsilon(tolerance));
    CHECK(sampleStates[2][2] == Approx(-0.48583171E+02 * nsx::deg).epsilon(tolerance));
}
