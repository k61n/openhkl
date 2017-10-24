#include <fstream>
#include <string>

#include <Eigen/Dense>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

const double tolerance=1e-2;

int main()
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* meta;
    Eigen::MatrixXi v;

    try {
        diff = nsx::Diffractometer::build("D10");
        dataf = factory.create("", "D10_ascii_example", diff);
        meta=dataf->getMetadata();

        NSX_CHECK_ASSERT(meta != nullptr);
        NSX_CHECK_ASSERT(meta->getKey<int>("nbang")==2);

        dataf->open();
        v = dataf->getFrame(0);
    }
    catch(std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    }
    catch (...) {
        NSX_FAIL("unknown exception while loading data");
    }

    // Check the total number of count in the frame 0
    NSX_CHECK_EQUAL(v.sum(),65);

    // Check the value of the monitor
    NSX_CHECK_CLOSE(meta->getKey<double>("monitor"),20000,tolerance);

    auto&& states = dataf->getInstrumentStates();

    NSX_CHECK_CLOSE(states[3].detector.getValues()[0],0.54347000E+05/1000.0*nsx::deg,tolerance);
    NSX_CHECK_CLOSE(states[2].sample.getValues()[0],0.26572000E+05/1000.0*nsx::deg,tolerance);
    NSX_CHECK_CLOSE(states[2].sample.getValues()[1],0.48923233E+02*nsx::deg,tolerance);
    NSX_CHECK_CLOSE(states[2].sample.getValues()[2],-0.48583171E+02*nsx::deg,tolerance);

    auto&& st = dataf->getInterpolatedState(0.0);
    NSX_CHECK_CLOSE(st.detector.getValues()[0],states[0].detector.getValues()[0],tolerance);
    NSX_CHECK_CLOSE(st.sample.getValues()[0],states[0].sample.getValues()[0],tolerance);

    st = dataf->getInterpolatedState(0.5);
    NSX_CHECK_CLOSE(st.detector.getValues()[0],states[0].detector.getValues()[0]+0.5*(states[1].detector.getValues()[0]-states[0].detector.getValues()[0]),tolerance);
    NSX_CHECK_CLOSE(st.sample.getValues()[0],states[0].sample.getValues()[0]+0.5*(states[1].sample.getValues()[0]-states[0].sample.getValues()[0]),tolerance);

    st = dataf->getInterpolatedState(2.3);
    NSX_CHECK_CLOSE(st.detector.getValues()[0],states[2].detector.getValues()[0]+0.3*(states[3].detector.getValues()[0]-states[2].detector.getValues()[0]),tolerance);
    NSX_CHECK_CLOSE(st.sample.getValues()[0],states[2].sample.getValues()[0]+0.3*(states[3].sample.getValues()[0]-states[2].sample.getValues()[0]),tolerance);

    meta = nullptr;
}
