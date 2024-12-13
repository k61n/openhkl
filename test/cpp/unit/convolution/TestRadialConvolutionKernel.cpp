//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestRadialConvolutionKernel.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/convolve/RadialConvolutionKernel.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <opencv2/core/hal/interface.h>

void check_matrix(const cv::Mat& mat, const cv::Mat& ref) {
    const double eps = 1.0e-5;

    CHECK(mat.rows == ref.rows);
    CHECK(mat.cols == ref.cols);

    for (std::size_t i = 0; i < mat.rows; ++i) {
        for (std::size_t j = 0; j < mat.cols; ++j) {
            CHECK_THAT(mat.at<double>(i, j), Catch::Matchers::WithinAbs(ref.at<double>(i, j), eps));
        }
    }
}

TEST_CASE("test/data/TestRadialConvolutionKernel.cpp", "")
{

    ohkl::RadialConvolutionKernel peak_kernel(0, 1);
    ohkl::RadialConvolutionKernel bkg_kernel(3, 5);

    cv::Mat peak_matrix = peak_kernel.matrix();
    cv::Mat bkg_matrix = bkg_kernel.matrix();

    cv::Ptr<cv::Formatter> fmt = cv::Formatter::get(cv::Formatter::FMT_DEFAULT);
    fmt->set64fPrecision(3);
    fmt->set32fPrecision(3);

    std::cout << fmt->format(peak_matrix) << std::endl;
    std::cout << fmt->format(bkg_matrix) << std::endl;

    cv::Mat peak_ref = (cv::Mat_<double>(3, 3) << 0, 0, 0, 0, 1, 0, 0, 0, 0);
    cv::Mat bkg_ref = (cv::Mat_<double>(11, 11) <<
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
                       0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
                       0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
                       0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
                       0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
                       0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
                       0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0,
                       0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
                       0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) / 44.0;
    std::cout << fmt->format(peak_ref) << std::endl;
    std::cout << fmt->format(bkg_ref) << std::endl;

    check_matrix(peak_matrix, peak_ref);
    check_matrix(bkg_matrix, bkg_ref);
}
