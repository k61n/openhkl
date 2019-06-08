#include <stdexcept>

#include "core/data/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/instrument/Experiment.h"

TEST_CASE("test/instrument/TestExperiment.cpp", "") {

    nsx::Experiment exp("my-exp", "D10");
    nsx::sptrDataSet data;

    NSX_CHECK_EQUAL(exp.name(), "my-exp");

    // Change the name of the experiment
    exp.setName("toto");
    NSX_CHECK_EQUAL(exp.name(), "toto");

    // Add some data
    try {
        data = nsx::sptrDataSet(
            nsx::DataReaderFactory().create("", "D10_ascii_example", exp.diffractometer()));
    } catch (std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        NSX_FAIL("unknown exception");
    }

    exp.addData(data);

    // Check that adding the same data is now taken into account
    exp.addData(data);
    exp.addData(data);
    exp.addData(data);

    NSX_CHECK_EQUAL(exp.data("D10_ascii_example")->filename(), "D10_ascii_example");

    // Remove the data from the experiment
    exp.removeData("D10_ascii_example");

    return 0;
}
