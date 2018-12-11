#include <vector>

#include <Eigen/Dense>

#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/Experiment.h>
#include <core/NSXTest.h>
#include <core/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::Experiment experiment("", "D10");

    nsx::sptrDataSet dataset(
        new nsx::DataSet("", "D10_ascii_example", experiment.diffractometer()));

    std::vector<Eigen::MatrixXi> frames;

    dataset->open();

    for (size_t i = 0; i < dataset->nFrames(); ++i) {
        frames.push_back(dataset->frame(i));
    }

    dataset->saveHDF5("D10_hdf5_example.h5");
    dataset->close();

    // read data back in and check that it agrees!
    dataset =
        std::make_shared<nsx::DataSet>("h5", "D10_hdf5_example.h5", experiment.diffractometer());

    NSX_CHECK_ASSERT(dataset != nullptr);

    for (size_t i = 0; i < dataset->nFrames(); ++i) {
        NSX_CHECK_ASSERT(dataset->frame(i) == frames[i]);
    }
    dataset->close();
}
