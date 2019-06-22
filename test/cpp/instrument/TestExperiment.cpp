#include "test/cpp/catch.hpp"

#include <stdexcept>

#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/algo/DataReaderFactory.h"

TEST_CASE("test/instrument/TestExperiment.cpp", "")
{
    nsx::Experiment exp("my-exp", "D10");
    nsx::sptrDataSet data;

    CHECK(exp.name() == "my-exp");

    // Change the name of the experiment
    exp.setName("toto");
    CHECK(exp.name() == "toto");

    // Add some data
    try {
        data = nsx::sptrDataSet(
            nsx::DataReaderFactory().create("", "D10_ascii_example", exp.diffractometer()));
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception");
    }

    exp.addData(data);

    // Check that adding the same data is now taken into account
    exp.addData(data);
    exp.addData(data);
    exp.addData(data);

    CHECK(exp.data("D10_ascii_example")->filename() == "D10_ascii_example");

    // Remove the data from the experiment
    exp.removeData("D10_ascii_example");
}
