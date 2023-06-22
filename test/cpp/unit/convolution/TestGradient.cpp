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

    ohkl::Experiment experiment("test", "BioDiff");
    ohkl::sptrDataSet data = std::make_shared<ohkl::DataSet>(
        ohkl::kw_datasetDefaultName, experiment.getDiffractometer());
    ohkl::DataReaderParameters data_params;

    data_params.wavelength = 2.669;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    data->addRawFrame(filename);
    data->finishRead();
    experiment.addData(data);

    Eigen::MatrixXd image = data->transformedFrame(0);
    ohkl::ImageGradient grad(image);
    double sum, sum_magnitude;

    grad.compute(ohkl::GradientKernel::CentralDifference, true);
    sum = grad.dx()->sum() + grad.dy()->sum();
    CHECK(sum < 1.0E-8);
    sum_magnitude = grad.magnitude().sum();
    std::cout << "Central difference: sum = " << sum << std::endl;

    grad.compute(ohkl::GradientKernel::CentralDifference, false);
    sum = grad.dx()->sum() + grad.dy()->sum();
    CHECK(sum < 1.0E-8);
    sum_magnitude -= grad.magnitude().sum();
    CHECK(sum_magnitude < 1.0E-8);
    std::cout << "Central difference (FFT): sum = " << sum << std::endl;
    std::cout << "Magnitude diff between real space and FFT: " << sum_magnitude << std::endl;;

    grad.compute(ohkl::GradientKernel::Sobel, true);
    sum = grad.dx()->sum() + grad.dy()->sum();
    CHECK(sum < 1.0E-8);
    sum_magnitude = grad.magnitude().sum();
    std::cout << "Sobel: sum = " << sum << std::endl;

    grad.compute(ohkl::GradientKernel::Sobel, false);
    sum = grad.dx()->sum() + grad.dy()->sum();
    CHECK(sum < 1.0E-8);
    sum_magnitude -= grad.magnitude().sum();
    CHECK(sum_magnitude < 1.0E-8);
    std::cout << "Sobel (FFT): sum = " << sum << std::endl;
    std::cout << "Magnitude diff between real space and FFT: " << sum_magnitude << std::endl;;
}
