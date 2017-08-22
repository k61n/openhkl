#define BOOST_TEST_MODULE "Test whether h,k,l peak falls into Data"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>

BOOST_AUTO_TEST_CASE(Test_Peak_Data)
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    nsx::MetaData* metadata;

    try {
        diff = nsx::Diffractometer::build("D9");
        dataf = factory.create("", "D9_ascii_example", diff);

        dataf->open();

        metadata = dataf->getMetadata();
        BOOST_CHECK(metadata != nullptr);
    }
    catch(std::exception& e) {
        BOOST_FAIL(std::string("caught exception: ") + e.what());
    }
    catch (...) {
        BOOST_FAIL("unknown exception");
    }
}
