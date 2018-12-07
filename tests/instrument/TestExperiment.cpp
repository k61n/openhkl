#include <core/DataSet.h>
#include <core/Experiment.h>
#include <core/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("my-exp","D10");

    NSX_CHECK_EQUAL(experiment.name(),"my-exp");

    // Change the name of the experiment
    experiment.setName("toto");

    NSX_CHECK_EQUAL(experiment.name(),"toto");

    nsx::sptrDataSet dataset(new nsx::DataSet("", "D10_ascii_example", experiment.diffractometer()));

    experiment.addData(dataset);

    // Check that adding the same data is now taken into account
    experiment.addData(dataset);
    experiment.addData(dataset);

    NSX_CHECK_EQUAL(experiment.data("D10_ascii_example")->filename(),"D10_ascii_example");

    // Remove the data from the experiment
    experiment.removeData("D10_ascii_example");

    return 0;
}

