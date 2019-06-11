#include "test/cpp/catch.hpp"
#include <vector>

#include <Eigen/Dense>

#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/experiment/DataReaderFactory.h"
#include "base/utils/Units.h"

TEST_CASE("test/data/TestHDF5IO.cpp", "")
{

    nsx::DataReaderFactory factory;
    nsx::Diffractometer* diffractometer;
    nsx::sptrDataSet dataf;

    std::vector<Eigen::MatrixXi> frames;

    try {
        diffractometer = nsx::Diffractometer::create("D10");
        dataf = factory.create("", "D10_ascii_example", diffractometer);
        dataf->open();

        for (size_t i = 0; i < dataf->nFrames(); ++i)
            frames.push_back(dataf->frame(i));

        dataf->saveHDF5("D10_hdf5_example.h5");
        dataf->close();

        // read data back in and check that it agrees!
        dataf = factory.create("h5", "D10_hdf5_example.h5", diffractometer);

        CHECK(dataf != nullptr);

        for (size_t i = 0; i < dataf->nFrames(); ++i)
            CHECK(dataf->frame(i) == frames[i]);
        dataf->close();
    } catch (std::exception& e) {
        FAIL(std::string("saveHDF5() threw exception: ") + e.what());
    } catch (...) {
        FAIL("saveHDF5() threw unknown exception");
    }
}
