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

#include "core/image/GradientFilter.h"
#include "test/cpp/catch.hpp"

#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
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
    const double ref_x = 284.8571428571;
    const double ref_y = 34907.4285714291;
    const double ref_norm = 12096030.4305160008;

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

    ohkl::CentralDifferenceFilter filter;
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
