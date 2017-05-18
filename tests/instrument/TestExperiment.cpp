#define BOOST_TEST_MODULE "Test Experiment"
#define BOOST_TEST_DYN_LINK

#include <memory>

#include <boost/test/unit_test.hpp>

#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/data/DataReaderFactory.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_Experiment)
{
    Experiment exp("my-exp","D10");
    std::shared_ptr<DataSet> data;

    BOOST_CHECK_EQUAL(exp.getName(),"my-exp");

    // Change the name of the experiment
    exp.setName("toto");
    BOOST_CHECK_EQUAL(exp.getName(),"toto");

    // The data must be empty at experiment creation
    BOOST_CHECK_EQUAL(exp.getDataNames().size(),0);

    // Add some data
    try {
        auto factory = DataReaderFactory::Instance();
        data = std::shared_ptr<DataSet>(factory->create("", "D10_ascii_example", exp.getDiffractometer()));
    }
    catch(std::exception& e) {
        BOOST_FAIL(std::string("caught exception: ") + e.what());
    }
    catch(...) {
        BOOST_FAIL("unknown exception");
    }

    exp.addData(data);
    BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);
    BOOST_CHECK_EQUAL(exp.getDataNames()[0],"D10_ascii_example");

    // Check that adding the same data is now taken into account
    exp.addData(data);
    exp.addData(data);
    exp.addData(data);
    BOOST_CHECK_EQUAL(exp.getDataNames().size(),1);

    BOOST_CHECK_EQUAL(exp.getData("D10_ascii_example")->getBasename(),"D10_ascii_example");

    // Remove the data from the experiment
    exp.removeData("D10_ascii_example");
    // The data must be empty again after data deletion
    BOOST_CHECK_EQUAL(exp.getDataNames().size(),0);
}

