//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestFilter.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataSet.h"
#include "test/cpp/catch.hpp"
#include "core/experiment/Experiment.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestFilter.cpp", "")
{
    const std::string filename = "CrChiA.ohkl";
    ohkl::Experiment experiment("test", "BioDiff");
    experiment.loadFromFile(filename);

    auto* peaks = experiment.getPeakCollection("found");
    int npeaks = peaks->numberOfPeaks();

    auto* filter = experiment.peakFilter();
    auto* flags = filter->flags();
    auto* params = filter->parameters();

    filter->resetFilterFlags();
    filter->resetFiltering(peaks);

    flags->d_range = true;
    params->d_min = 2;
    params->d_max = 40;
    filter->filter(peaks);
    std::cout << peaks->numberCaughtByFilter() << "/" << npeaks << " in d range 2 - 40"
              << std::endl;
    CHECK(peaks->numberCaughtByFilter() == 404);

    filter->resetFilterFlags();
    filter->resetFiltering(peaks);

    flags->strength = true;
    params->strength_min = 5;
    params->strength_max = 10000;
    filter->filter(peaks);
    std::cout << peaks->numberCaughtByFilter() << "/" << npeaks << " with strength 5 - 10000"
              << std::endl;
    CHECK(peaks->numberCaughtByFilter() == 413);

    filter->resetFilterFlags();
    filter->resetFiltering(peaks);

    flags->frames = true;
    params->first_frame = 3;
    params->last_frame = 10;
    filter->filter(peaks);
    std::cout << peaks->numberCaughtByFilter() << "/" << npeaks << " in frames 3 - 10"
              << std::endl;
    CHECK(peaks->numberCaughtByFilter() == 222);

    filter->resetFilterFlags();
    filter->resetFiltering(peaks);

    flags->rejection_flag = true;
    params->rejection_flag = ohkl::RejectionFlag::NotRejected;
    filter->filter(peaks);
    std::cout << peaks->numberCaughtByFilter() << "/" << npeaks << " not rejected"
              << std::endl;
    CHECK(peaks->numberCaughtByFilter() == 247);

    filter->resetFilterFlags();
    filter->resetFiltering(peaks);

    flags->gradient = true;
    params->gradient_min = 0;
    params->gradient_max = 6.5;
    filter->filter(peaks);
    std::cout << peaks->numberCaughtByFilter() << "/" << npeaks << " with gradient < 6.5"
              << std::endl;
    CHECK(peaks->numberCaughtByFilter() == 492);
}
