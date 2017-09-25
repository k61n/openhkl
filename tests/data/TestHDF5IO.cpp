#include <vector>

#include <Eigen/Dense>

#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/utils/NSXTest.h>
#include <nsxlib/utils/Units.h>

int main()
{
    nsx::DataReaderFactory factory;
    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;

    std::vector<Eigen::MatrixXi> frames;

    try {
        diff = nsx::Diffractometer::build("D10");
        dataf = factory.create("", "D10_ascii_example", diff);
        dataf->open();

        for (size_t i = 0; i < dataf->getNFrames(); ++i) {
            frames.push_back(dataf->getFrame(i));
        }

        dataf->saveHDF5("D10_hdf5_example.h5");
        dataf->close();
        
        // read data back in and check that it agrees!
        dataf = factory.create("h5", "D10_hdf5_example.h5", diff);

        NSX_CHECK_ASSERT(dataf != nullptr);

        for (size_t i = 0; i < dataf->getNFrames(); ++i) {
            NSX_CHECK_ASSERT(dataf->getFrame(i) == frames[i]);
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
