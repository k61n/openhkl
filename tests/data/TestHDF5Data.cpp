#include <Eigen/Dense>

#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("","BioDiff2500");

    nsx::DataSet dataset("hdf", "H5_example.hdf", experiment.diffractometer());

    dataset.open();
    Eigen::MatrixXi v = dataset.frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 1282584565);

    dataset.close();
}
