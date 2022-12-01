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
    experiment.addData(data);

    ohkl::ImageGradient grad{data->transformedFrame(0)};
    double sum;

    grad.compute(ohkl::GradientKernel::CentralDifference, true);
    sum = grad.dx()->sum() + grad.dy()->sum();
    std::cout << "Central difference: " << sum << std::endl;

    grad.compute(ohkl::GradientKernel::CentralDifference, false);
    sum = grad.dx()->sum() + grad.dy()->sum();
    std::cout << "Central difference (FFT): " << sum << std::endl;

    grad.compute(ohkl::GradientKernel::Sobel, true);
    sum = grad.dx()->sum() + grad.dy()->sum();
    std::cout << "Sobel: " << sum << std::endl;

    grad.compute(ohkl::GradientKernel::Sobel, false);
    sum = grad.dx()->sum() + grad.dy()->sum();
    std::cout << "Sobel (FFT): " << sum << std::endl;
}
