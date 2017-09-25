#include <stdexcept>

#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/data/MetaData.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/utils/NSXTest.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-2;

int main()
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* meta;
    Eigen::MatrixXi v;

    try {
        diff = nsx::Diffractometer::build("D9");
        dataf = factory.create("", "D9_QSCAN", diff);
        meta=dataf->getMetadata();

        NSX_CHECK_ASSERT(meta->getKey<int>("nbang")==4);

        dataf->open();
        v = dataf->getFrame(0);
    }
    catch (std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    }
    catch(...) {
        NSX_FAIL("unknown exception");
    }

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(),5.90800000e+03);

    // Check the value of the monitor
    NSX_CHECK_CLOSE(meta->getKey<double>("monitor"),3.74130000e+04,tolerance);

    dataf->close();
}
