//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestInstrumentState.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/raw/DataKeys.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include "core/loader/IDataReader.h"

#include <iostream>

namespace ohkl {

TEST_CASE("test/instrument/TestInstrumentState.cpp", "")
{
    ohkl::Experiment experiment("gal3", "BioDiff");

    const ohkl::sptrDataSet dataset_ptr { std::make_shared<ohkl::DataSet>
          ("gal3", experiment.getDiffractometer()) };

    dataset_ptr->addDataFile("gal3.hdf", ohkl::DataFormat::OHKL);
    dataset_ptr->finishRead();
    experiment.addData(dataset_ptr);

    auto detectorAngles = dataset_ptr->diffractometer()->detectorAngles();
    auto sampleAngles = dataset_ptr->diffractometer()->sampleAngles();

    int good_states = 0;
    int total_states = 0;
    for (size_t i = 0; i < 100 * (dataset_ptr->nFrames() - 1); ++i) {
        double frame = double(i) / 100.0;
        ++total_states;
        try {
            auto state = InterpolatedState::interpolate(dataset_ptr->instrumentStates(), frame);
            ++good_states;
        } catch (std::range_error& e) {
            std::cout << e.what() << std::endl;
            continue;
        }
        // auto lframe = std::lround(std::floor(frame));
    }
    std::cout << good_states << "/" << total_states << " good states" << std::endl;
    CHECK(good_states == 3000);
}

}
