#include <Eigen/Dense>

#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("", "BioDiff2500");

    nsx::DataSet dataset("hdf", "H5_example.hdf", experiment.diffractometer());

    dataset.open();
    Eigen::MatrixXi v = dataset.frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 1282584565);

    dataset.close();

    nsx::DataSet dataset1(dataset);

    dataset1.open();

    Eigen::MatrixXi v1 = dataset1.frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v1.sum(), v.sum());

    dataset1.close();
}
