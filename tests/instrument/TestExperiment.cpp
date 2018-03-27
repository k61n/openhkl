#include <stdexcept>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment exp("my-exp","D10");
    nsx::sptrDataSet data;

    NSX_CHECK_EQUAL(exp.getName(),"my-exp");

    // Change the name of the experiment
    exp.setName("toto");
    NSX_CHECK_EQUAL(exp.getName(),"toto");

    // The data must be empty at experiment creation
    NSX_CHECK_EQUAL(exp.getDataNames().size(),0);

    // Add some data
    try {
        data = nsx::sptrDataSet(nsx::DataReaderFactory().create("", "D10_ascii_example", exp.getDiffractometer()));
    }
    catch(std::exception& e) {
        NSX_FAIL(std::string("caught exception: ") + e.what());
    }
    catch(...) {
        NSX_FAIL("unknown exception");
    }

    exp.addData(data);
    NSX_CHECK_EQUAL(exp.getDataNames().size(),1);
    NSX_CHECK_EQUAL(exp.getDataNames()[0],"D10_ascii_example");

    // Check that adding the same data is now taken into account
    exp.addData(data);
    exp.addData(data);
    exp.addData(data);
    NSX_CHECK_EQUAL(exp.getDataNames().size(),1);

    NSX_CHECK_EQUAL(exp.getData("D10_ascii_example")->filename(),"D10_ascii_example");

    // Remove the data from the experiment
    exp.removeData("D10_ascii_example");
    // The data must be empty again after data deletion
    NSX_CHECK_EQUAL(exp.getDataNames().size(),0);

    return 0;
}

