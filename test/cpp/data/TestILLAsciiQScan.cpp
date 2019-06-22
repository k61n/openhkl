#include "test/cpp/catch.hpp"
#include <stdexcept>

#include <Eigen/Dense>

#include "base/utils/Units.h"
#include "core/algo/DataReaderFactory.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"

const double tolerance = 1e-2;

TEST_CASE("test/data/TestILLAsciiQScan.cpp", "")
{
    nsx::DataReaderFactory factory;
    nsx::Diffractometer* diffractometer;
    nsx::sptrDataSet dataf;
    Eigen::MatrixXi v;

    nsx::MetaData metadata;

    try {
        diffractometer = nsx::Diffractometer::create("D9");
        dataf = factory.create("", "D9_QSCAN", diffractometer);

        metadata = dataf->reader()->metadata();

        CHECK(metadata.key<int>("nbang") == 4);

        dataf->open();
        v = dataf->frame(0);
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception");
    }

    // Check the total number of count in the frame 0
    CHECK(v.sum() == 5.90800000e+03);

    // Check the value of the monitor
    CHECK(metadata.key<double>("monitor") == Approx(3.74130000e+04).epsilon(tolerance));

    dataf->close();
}
