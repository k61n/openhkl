#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/utils/NSXTest.h>

int main()
{
    nsx::DataReaderFactory factory;

    auto diff = nsx::Diffractometer::build("BioDiff2500");
    auto dataf = factory.create("hdf", "H5_example.hdf", diff);

    dataf->open();
    Eigen::MatrixXi v = dataf->getFrame(0);

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(), 1282584565);

    dataf->close();
}
