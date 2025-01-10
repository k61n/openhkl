//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestSpotFinder.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/shape/PeakFilter.h"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

//#define OUTPUT_INTERMEDIATE 1


TEST_CASE("test/data/TestSpotFinder.cpp", "")
{
    const std::string filename = "CrChiA_c01runab_28603.raw";
    ohkl::Experiment experiment("CrChiA", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("CrChiA", experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.667;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    data->addFrame(filename, ohkl::DataFormat::RAW);
    data->finishRead();

    ohkl::PeakFinder2D* finder = experiment.peakFinder2D();
    auto* finder_params = finder->parameters();
    finder_params->threshold = 80;
    finder_params->kernel = ohkl::ImageFilterType::Annular;
    std::map<std::string, double> filter_params = {{"r1", 5}, {"r2", 10}, {"r3", 15}};

    finder->setFilterParameters(filter_params);
    finder->setData(data);
    finder->find(0);

    auto* keypoints = finder->keypoints();
    std::cout << keypoints->frame(0)->size() << " keypoints found" << std::endl;
    std::vector<ohkl::Peak3D*> found_peaks = finder->getPeakList(0);
    std::cout << found_peaks.size() << " peaks found" << std::endl;

    CHECK(found_peaks.size() > 140);
}
