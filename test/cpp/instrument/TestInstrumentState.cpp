//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/instrument/TestInstrumentState.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/algo/DataReaderFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentState.h"
#include "core/raw/IDataReader.h"

namespace nsx {

class UnitTest_DataSet {
 public:
    static int run();
};

} // namespace nsx

int nsx::UnitTest_DataSet::run()
{
    nsx::DataReaderFactory factory;

    nsx::Experiment experiment("test", "BioDiff2500");

    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", experiment.getDiffractometer()));

    experiment.addData(dataf);

    auto detectorStates = dataf->_reader->detectorStates();
    auto sampleStates = dataf->_reader->sampleStates();

    int good_states = 0;
    int total_states = 0;
    for (size_t i = 0; i < 100 * (dataf->nFrames() - 1); ++i) {
        double frame = double(i) / 100.0;
        ++total_states;
        try {
            auto state = dataf->instrumentStates().interpolate(frame);
            ++good_states;
        } catch (std::range_error& e) {
            std::cout << e.what() << std::endl;
            continue;
        }
        // auto lframe = std::lround(std::floor(frame));
    }
    std::cout << good_states << "/" << total_states << " good states" << std::endl;
    CHECK(good_states == 2901);
    return 0;
}

TEST_CASE("test/instrument/TestInstrumentState.cpp", "")
{
    nsx::UnitTest_DataSet::run();
}
