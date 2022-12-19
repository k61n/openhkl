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

#include "core/convolve/AnnularConvolver.h"
#include "core/convolve/Convolver.h"
#include "core/loader/RawDataReader.h"
#include "core/shape/PeakCollection.h"
#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/shape/PeakFilter.h"

//#define OUTPUT_INTERMEDIATE 1


TEST_CASE("test/data/TestPeakFinder2D.cpp", "")
{
    const std::vector<std::string> filenames = {
        "CrChiA_c01runab_28603.raw", "CrChiA_c01runab_28604.raw", "CrChiA_c01runab_28605.raw",
        "CrChiA_c01runab_28606.raw", "CrChiA_c01runab_28607.raw", "CrChiA_c01runab_28608.raw",
        "CrChiA_c01runab_28609.raw", "CrChiA_c01runab_28610.raw", "CrChiA_c01runab_28611.raw",
        "CrChiA_c01runab_28612.raw", "CrChiA_c01runab_28613.raw", "CrChiA_c01runab_28614.raw",
        "CrChiA_c01runab_28615.raw", "CrChiA_c01runab_28616.raw", "CrChiA_c01runab_28617.raw"};

    ohkl::Experiment experiment("test", "BioDiff2500");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>(ohkl::kw_datasetDefaultName, experiment.getDiffractometer());

    ohkl::RawDataReaderParameters data_params;
    data_params.wavelength = 2.667;
    data_params.delta_omega = 0.3;
    data->setRawReaderParameters(data_params);
    for (const auto& filename : filenames)
        data->addRawFrame(filename);
    data->finishRead();
    experiment.addData(data);

    // mask edges and beam stop
    ohkl::AABB box1 = {{0, 0, 0}, {300, 900, 15}};
    ohkl::AABB box2 = {{2200, 0, 0}, {2500, 900, 15}};
    ohkl::AABB ellipse = {{1200, 400, 0}, {1300, 500, 15}};
    data->addMask(new ohkl::BoxMask(box1));
    data->addMask(new ohkl::BoxMask(box2));
    data->addMask(new ohkl::EllipseMask(ellipse));

    ohkl::PeakFinder* finder = experiment.peakFinder();
    auto* finder_params = finder->parameters();
    finder_params->frames_begin = 0;
    finder_params->frames_end = -1;
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->threshold = 80;
    finder->setConvolver(ohkl::AnnularConvolver());
    finder->find(experiment.getAllData()[0]);

    std::vector<ohkl::Peak3D*> found_peaks = finder->currentPeaks();
    std::map<ohkl::Peak3D*, ohkl::RejectionFlag> tmp_map;
    data->maskPeaks(found_peaks, tmp_map);

    experiment.acceptFoundPeaks("found");
    ohkl::PeakCollection* peaks = experiment.getPeakCollection("found");

    ohkl::PeakFilter* filter = experiment.peakFilter();
    filter->flags()->selected = false;
    filter->flags()->rejection_flag = true;
    filter->parameters()->rejection_flag = ohkl::RejectionFlag::Masked;
    filter->filter(peaks);

    int npeaks = peaks->numberOfPeaks();
    int nrejected = peaks->numberCaughtByFilter();

    std::cout << nrejected << "/" << npeaks << " peaks masked" << std::endl;
    CHECK(nrejected >= 38);
}
