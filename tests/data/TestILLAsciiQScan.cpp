#include <stdexcept>

#include <Eigen/Dense>

#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/data/IDataReader.h"
#include "core/data/MetaData.h"
#include "core/utils/Units.h"

const double tolerance = 1e-2;

NSX_INIT_TEST

int main()
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

        NSX_CHECK_ASSERT(metadata.key<int>("nbang") == 4);

        dataf->open();
        v = dataf->frame(0);
    } catch (std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        NSX_FAIL("unknown exception");
    }

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 5.90800000e+03);

    // Check the value of the monitor
    NSX_CHECK_CLOSE(metadata.key<double>("monitor"), 3.74130000e+04, tolerance);

    dataf->close();
}
