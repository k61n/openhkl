//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestCentralDifferenceFilter.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
#include "core/image/GradientFilterFactory.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/image/CentralDifferenceFilter.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

TEST_CASE("test/data/TestCentralDifferenceFilter.cpp", "")
{
    const double eps = 1.0e-5;
    const double cd_ref_x = 284.8571428571;
    const double sobel_ref_x = 2278.8571428572;

    const std::string filename = "CrChiA_c01runab_28603.raw";
    ohkl::Experiment experiment("CrChiA", "BioDiff");

    const ohkl::sptrDataSet data =
        std::make_shared<ohkl::SingleFrame>("CrChiA", experiment.getDiffractometer());

    ohkl::DataReaderParameters data_params;
    data_params.data_format = ohkl::DataFormat::RAW;
    data_params.wavelength = 2.667;
    data_params.delta_omega = 0.3;
    data->setImageReaderParameters(data_params);
    data->addFrame(filename, ohkl::DataFormat::RAW);
    data->finishRead();

    ohkl::RealMatrix image = data->transformedFrame(0);

    ohkl::GradientFilterFactory factory;
    ohkl::GradientFilter* cd_filter = factory.create("Central difference");

    cv::Mat cd_filtered_image;
    cd_filter->setImage(image);
    cd_filter->filter(ohkl::GradientDirection::X);
    cd_filtered_image = cd_filter->cvFilteredImage();
    double sum_x = cv::sum(cd_filtered_image)[0];
    std::cout << "sum_x = " << sum_x << std::endl;
    CHECK_THAT(sum_x, Catch::Matchers::WithinAbs(cd_ref_x, eps));

    ohkl::GradientFilter* sobel_filter = factory.create("Sobel 3");

    cv::Mat sobel_filtered_image;
    sobel_filter->setImage(image);
    sobel_filter->filter(ohkl::GradientDirection::X);
    sobel_filtered_image = sobel_filter->cvFilteredImage();
    sum_x = cv::sum(sobel_filtered_image)[0];
    std::cout << "sum_x = " << sum_x << std::endl;
    CHECK_THAT(sum_x, Catch::Matchers::WithinAbs(sobel_ref_x, eps));
}
