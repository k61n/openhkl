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
        "p11202_00009983.tiff", "p11202_00009984.tiff", "p11202_00009985.tiff",
        "p11202_00009986.tiff", "p11202_00009987.tiff", "p11202_00009988.tiff",
        "p11202_00009989.tiff", "p11202_00009990.tiff", "p11202_00009991.tiff",
        "p11202_00009992.tiff", "p11202_00009993.tiff", "p11202_00009994.tiff",
        "p11202_00009995.tiff", "p11202_00009996.tiff", "p11202_00009997.tiff",
        "p11202_00009998.tiff", "p11202_00009999.tiff", "p11202_00010000.tiff",
        "p11202_00010001.tiff", "p11202_00010002.tiff"};

    ohkl::Experiment experiment("Trypsin", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::DataSet>("Trypsin", experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::TIFF;
    data_params.wavelength = 2.67;
    data_params.delta_omega = 0.4;
    data_params.rebin_size = 2;
    data->setImageReaderParameters(data_params);
    for (const auto& file : filenames)
        data->addFrame(file, ohkl::DataFormat::TIFF);
    data->finishRead();
    experiment.addData(data);

    ohkl::PeakFinder* finder = experiment.peakFinder();
    auto* finder_params = finder->parameters();
    finder_params->first_frame = 0;
    finder_params->last_frame = -1;
    finder_params->minimum_size = 30;
    finder_params->maximum_size = 10000;
    finder_params->peak_end = 1.0;
    finder_params->threshold = 150;
    finder_params->filter = "Annular";
    std::map<std::string, double> filter_params = {{"r1", 5}, {"r2", 10}, {"r3", 15}};

    finder->setFilterParameters(filter_params);
    finder->find(experiment.getAllData()[0]);
    std::cout << finder->numberFound() << " peaks found" << std::endl;

    CHECK(finder->numberFound() == 698);
}
