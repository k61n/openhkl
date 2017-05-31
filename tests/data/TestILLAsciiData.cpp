#define BOOST_TEST_MODULE "Test ILL Ascii Data"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <string>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/instrument/InstrumentState.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/MetaData.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/data/DataSet.h>

const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_ILL_Data)
{
    nsx::DataReaderFactory factory;
    nsx::DiffractometerStore* ds;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* meta;
    Eigen::MatrixXi v;

    try {
        ds = nsx::DiffractometerStore::Instance();
        diff = nsx::sptrDiffractometer(ds->buildDiffractometer("D10"));
        dataf = factory.create("", "D10_ascii_example", diff);
        meta=dataf->getMetadata();

        BOOST_CHECK(meta != nullptr);
        BOOST_CHECK(meta->getKey<int>("nbang")==2);

        dataf->open();
        v = dataf->getFrame(0);
    }
    catch(std::exception& e) {
        BOOST_FAIL(std::string("caught exception: ") + e.what());
    }
    catch (...) {
        BOOST_FAIL("unknown exception while loading data");
    }

    // Check the total number of count in the frame 0
    BOOST_CHECK_EQUAL(v.sum(),65);

    // Check the value of the monitor
    BOOST_CHECK_CLOSE(meta->getKey<double>("monitor"),20000,tolerance);

    auto&& states = dataf->getInstrumentStates();

    BOOST_CHECK_CLOSE(states[3].detector.getValues()[0],0.54347000E+05/1000.0*nsx::deg,tolerance);
    BOOST_CHECK_CLOSE(states[2].sample.getValues()[0],0.26572000E+05/1000.0*nsx::deg,tolerance);
    BOOST_CHECK_CLOSE(states[2].sample.getValues()[1],0.48923233E+02*nsx::deg,tolerance);
    BOOST_CHECK_CLOSE(states[2].sample.getValues()[2],-0.48583171E+02*nsx::deg,tolerance);

    auto&& st = dataf->getInterpolatedState(0.0);
    BOOST_CHECK_CLOSE(st.detector.getValues()[0],states[0].detector.getValues()[0],tolerance);
    BOOST_CHECK_CLOSE(st.sample.getValues()[0],states[0].sample.getValues()[0],tolerance);

    st = dataf->getInterpolatedState(0.5);
    BOOST_CHECK_CLOSE(st.detector.getValues()[0],states[0].detector.getValues()[0]+0.5*(states[1].detector.getValues()[0]-states[0].detector.getValues()[0]),tolerance);
    BOOST_CHECK_CLOSE(st.sample.getValues()[0],states[0].sample.getValues()[0]+0.5*(states[1].sample.getValues()[0]-states[0].sample.getValues()[0]),tolerance);

    st = dataf->getInterpolatedState(2.3);
    BOOST_CHECK_CLOSE(st.detector.getValues()[0],states[2].detector.getValues()[0]+0.3*(states[3].detector.getValues()[0]-states[2].detector.getValues()[0]),tolerance);
    BOOST_CHECK_CLOSE(st.sample.getValues()[0],states[2].sample.getValues()[0]+0.3*(states[3].sample.getValues()[0]-states[2].sample.getValues()[0]),tolerance);

    meta = nullptr;
}
