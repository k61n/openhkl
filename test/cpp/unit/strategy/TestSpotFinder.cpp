//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestPeakFinder2D.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/convolve/Convolver.h"
#include "core/loader/RawDataReader.h"
#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/raw/DataKeys.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakFilter.h"

//#define OUTPUT_INTERMEDIATE 1


TEST_CASE("test/data/TestPeakFinder2D.cpp", "")
{
    const std::string filename = "CrChiA_c01runab_28603.raw";
    ohkl::Experiment experiment("test", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>(ohkl::kw_datasetDefaultName, experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.667;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    data->addRawFrame(filename);
    data->finishRead();

    ohkl::PeakFinder2D* finder = experiment.peakFinder2D();
    auto* finder_params = finder->parameters();
    finder_params->threshold = 80;
    finder->setData(data);
    finder->setConvolver(ohkl::ConvolutionKernelType::Annular);
    finder->find(0);

    auto* keypoints = finder->keypoints();
    std::cout << keypoints->frame(0)->size() << " keypoints found" << std::endl;
    std::vector<ohkl::Peak3D*> found_peaks = finder->getPeakList(0);
    std::cout << found_peaks.size() << " peaks found" << std::endl;

    CHECK(found_peaks.size() > 140);
}
