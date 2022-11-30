//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/peak_find/TestGradient.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/data/DataSet.h"
#include "core/data/ImageGradient.h"
#include "core/experiment/Experiment.h"
#include "core/loader/RawDataReader.h"

#include "test/cpp/catch.hpp"

#include <iostream>

TEST_CASE("test/peak_find/TestGradient.cpp", "")
{
    const std::string filename = "CrChiA_c01runab_28603.raw";

    ohkl::Experiment experiment("test", "BioDiff2500");
    ohkl::sptrDataSet data = std::make_shared<ohkl::DataSet>(
        ohkl::kw_datasetDefaultName, experiment.getDiffractometer());
    ohkl::RawDataReaderParameters data_params;

    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setRawReaderParameters(data_params);
    data->addRawFrame(filename);
    data->finishRead();

    Eigen::MatrixXi image_data = data->frame(0);

    ohkl::ImageGradient* grad1 = data->imageGradient(0, ohkl::GradientKernel::CentralDifference, true);
    double sum1 = grad1->dx().sum() + grad1->dy().sum();
    std::cout << "Central difference: " << sum1 << std::endl;

    ohkl::ImageGradient* grad2 =
        data->imageGradient(0, ohkl::GradientKernel::CentralDifference, false);
    double sum2 = grad2->dx().sum() + grad2->dy().sum();
    std::cout << "Central difference (FFT): " << sum2 << std::endl;

    ohkl::ImageGradient* grad3 =
        data->imageGradient(0, ohkl::GradientKernel::Sobel, true);
    double sum3 = grad3->dx().sum() + grad3->dy().sum();
    std::cout << "Sobel: " << sum3 << std::endl;

    ohkl::ImageGradient* grad4 = data->imageGradient(0, ohkl::GradientKernel::Sobel, false);
    double sum4 = grad4->dx().sum() + grad4->dy().sum();
    std::cout << "Sobel (FFT): " << sum4 << std::endl;
}
