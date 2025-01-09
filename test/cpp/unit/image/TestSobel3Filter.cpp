//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestSobel3Filter.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/image/GradientFilter.h"
#include "test/cpp/catch.hpp"

#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/image/Sobel3Filter.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

TEST_CASE("test/data/TestSobel3Filter.cpp", "")
{
    const double eps = 1.0e-5;
    const double ref_x = 2278.8571428572;
    const double ref_y = 279259.428571402;
    const double ref_norm = 64467949.1313979626;

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

    ohkl::Sobel3Filter filter;
    cv::Mat filtered_image;
    filter.setImage(image);
    filter.filter(ohkl::GradientDirection::X);
    filtered_image = filter.cvFilteredImage();
    double sum_x = cv::sum(filtered_image)[0];

    // cv::imshow("x gradient", filtered_image);
    // cv::waitKey();

    filter.filter(ohkl::GradientDirection::Y);
    filtered_image = filter.cvFilteredImage();
    double sum_y = cv::sum(filtered_image)[0];

    // cv::imshow("x gradient", filtered_image);
    // cv::waitKey();

    filter.filter();
    filtered_image = filter.cvFilteredImage();
    double sum_norm = cv::sum(filtered_image)[0];

    // cv::imshow("magnitude gradient", filtered_image);
    // cv::waitKey();

    std::cout << "sum_x = " << sum_x << std::endl;
    std::cout << "sum_y = " << sum_y << std::endl;
    std::cout << "sum_norm = " << sum_norm << std::endl;
    CHECK_THAT(sum_x, Catch::Matchers::WithinAbs(ref_x, eps));
    CHECK_THAT(sum_y, Catch::Matchers::WithinAbs(ref_y, eps));
    CHECK_THAT(sum_norm, Catch::Matchers::WithinAbs(ref_norm, eps));
}
