#include <Eigen/Dense>

#include <core/DataReaderFactory.h>
#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::DataReaderFactory factory;

    nsx::Diffractometer *diffractometer = nsx::Diffractometer::create("BioDiff2500");
    auto dataf = factory.create("hdf", "H5_example.hdf", diffractometer);

    dataf->open();
    Eigen::MatrixXi v = dataf->frame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
}
