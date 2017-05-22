#define BOOST_TEST_MODULE "Test whether h,k,l peak falls into Data"
#define BOOST_TEST_DYN_LINK

#include <memory>

#include <boost/test/unit_test.hpp>

#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>

using namespace nsx;

// const double tolerance=1e-2;

BOOST_AUTO_TEST_CASE(Test_Peak_Data)
{
    DataReaderFactory factory;
    DiffractometerStore* ds;
    std::shared_ptr<Diffractometer> diff;
    std::shared_ptr<DataSet> dataf;
    MetaData* metadata;

    try {
        ds = DiffractometerStore::Instance();
        diff = std::shared_ptr<Diffractometer>(ds->buildDiffractometer("D9"));
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
