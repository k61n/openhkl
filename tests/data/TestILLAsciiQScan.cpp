#include <stdexcept>

#include <Eigen/Dense>

#include <nsxlib/ComponentState.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

const double tolerance=1e-2;

int main()
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* meta(nullptr);
    Eigen::MatrixXi v;

    try {
        diff = nsx::Diffractometer::build("D9");
        dataf = factory.create("", "D9_QSCAN", diff);
        meta=dataf->metadata();

        NSX_CHECK_ASSERT(meta->getKey<int>("nbang")==4);

        dataf->open();
        v = dataf->frame(0);
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
