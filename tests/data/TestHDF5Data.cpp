#include <Eigen/Dense>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::DataReaderFactory factory;

    auto diff = nsx::Diffractometer::create("BioDiff2500");
    auto dataf = factory.create("hdf", "H5_example.hdf", diff);

    dataf->open();
    Eigen::MatrixXi v = dataf->frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
}
