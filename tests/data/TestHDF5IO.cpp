#include <vector>

#include <Eigen/Dense>

#include <core/DataReaderFactory.h>
#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/NSXTest.h>
#include <core/Units.h>

NSX_INIT_TEST

int main()
{
    nsx::DataReaderFactory factory;
    nsx::Diffractometer *diffractometer;
    nsx::sptrDataSet dataf;

    std::vector<Eigen::MatrixXi> frames;

    try {
        diffractometer = nsx::Diffractometer::create("D10");
        dataf = factory.create("", "D10_ascii_example", diffractometer);
        dataf->open();

        for (size_t i = 0; i < dataf->nFrames(); ++i) {
            frames.push_back(dataf->frame(i));
        }

        dataf->saveHDF5("D10_hdf5_example.h5");
        dataf->close();
        
        // read data back in and check that it agrees!
        dataf = factory.create("h5", "D10_hdf5_example.h5", diffractometer);

        NSX_CHECK_ASSERT(dataf != nullptr);

        for (size_t i = 0; i < dataf->nFrames(); ++i) {
            NSX_CHECK_ASSERT(dataf->frame(i) == frames[i]);
        }
        dataf->close();
    }
    catch (std::exception& e) {
        NSX_FAIL(std::string("saveHDF5() threw exception: ") + e.what());
    }
    catch(...) {
        NSX_FAIL("saveHDF5() threw unknown exception");
    }  
}
