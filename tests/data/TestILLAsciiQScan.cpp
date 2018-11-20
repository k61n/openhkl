#include <stdexcept>

#include <Eigen/Dense>

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

const double tolerance=1e-2;

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("","D9");

    Eigen::MatrixXi v;

    nsx::sptrDataSet dataset(new nsx::DataSet("", "D9_QSCAN", experiment.diffractometer()));

    auto metadata = dataset->reader()->metadata();

    NSX_CHECK_ASSERT(metadata.key<int>("nbang")==4);

    dataset->open();
    v = dataset->frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(),5.90800000e+03);

    // Check the value of the monitor
    NSX_CHECK_CLOSE(metadata.key<double>("monitor"),3.74130000e+04,tolerance);

    dataset->close();
}
