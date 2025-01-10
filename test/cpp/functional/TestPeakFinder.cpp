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
#include "core/loader/IDataReader.h"
#include "test/cpp/catch.hpp"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder.h"
#include "core/loader/RawDataReader.h"
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
    const std::vector<std::string> filenames = {
        "CrChiA_c01runab_28603.raw",
        "CrChiA_c01runab_28604.raw",
        "CrChiA_c01runab_28605.raw",
        "CrChiA_c01runab_28606.raw",
        "CrChiA_c01runab_28607.raw",
        "CrChiA_c01runab_28608.raw",
        "CrChiA_c01runab_28609.raw",
        "CrChiA_c01runab_28610.raw",
        "CrChiA_c01runab_28611.raw",
        "CrChiA_c01runab_28612.raw",
        "CrChiA_c01runab_28613.raw",
        "CrChiA_c01runab_28614.raw",
        "CrChiA_c01runab_28615.raw",
        "CrChiA_c01runab_28616.raw",
        "CrChiA_c01runab_28617.raw"};
    ohkl::Experiment experiment("CrChiA", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("CrChiA", experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    for (const auto& file : filenames)
        data->addFrame(file, ohkl::DataFormat::RAW);
    data->finishRead();
    experiment.addData(data);

    ohkl::PeakFinder* finder = experiment.peakFinder();
    auto* finder_params = finder->parameters();
    finder_params->first_frame = 0;
    finder_params->last_frame = -1;
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->threshold = 80;
    finder_params->filter = "Annular";
    std::map<std::string, double> filter_params = {{"r1", 5}, {"r2", 10}, {"r3", 15}};

    finder->setFilterParameters(filter_params);
    finder->find(experiment.getAllData()[0]);
    std::cout << finder->numberFound() << " peaks found" << std::endl;

    CHECK(finder->numberFound() == 493);
}
