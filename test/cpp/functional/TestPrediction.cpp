//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestPeakFinder.cpp
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
#include "core/experiment/PeakFinder.h"
#include "core/loader/RawDataReader.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"


#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

TEST_CASE("test/data/TestPeakFinder.cpp", "")
{
    const std::string filename = "CrChiA.ohkl";
    ohkl::Experiment experiment("test", "BioDiff");
    experiment.loadFromFile(filename);

    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("accepted");
    ohkl::sptrDataSet data = experiment.getData("testdata");

    auto* predictor = experiment.predictor();
    auto* params = predictor->parameters();
    params->d_min = 1.5;
    params->d_max = 50.0;
    predictor->predictPeaks(data, cell);

    int expected_n_peaks = 5054;
    int eps = 10;

    std::cout << predictor->numberOfPredictedPeaks() << " peaks predicted" << std::endl;
    CHECK(predictor->numberOfPredictedPeaks() >= expected_n_peaks - eps);
    CHECK(predictor->numberOfPredictedPeaks() <= expected_n_peaks + eps);
}
