//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestAnnularConvolutionKernel.cpp
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
#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

TEST_CASE("test/data/TestAnnularConvolutionKernel.cpp", "")
{
    const double eps = 1.0e-5;

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

    cv::Mat cv_image;
    cv::eigen2cv(image, cv_image);

    cv::Mat cv_image_view;
    cv::normalize(cv_image, cv_image_view, 0, 255, cv::NORM_MINMAX);
    cv::log(cv_image_view, cv_image_view);
    cv::imshow("cv_image_view", cv_image_view);
    cv::waitKey();

    // std::map<std::string, double> params1 = {{"r_in", 0}, {"r_out", 5}};
    // ohkl::RadialConvolutionKernel kernel1(params1);
    // cv::Mat cv_image_filter1;
    // cv::Point anchor(-1, -1);
    // cv::filter2D(cv_image, cv_image_filter1, -1, kernel1.matrix(), anchor);

    // cv::Ptr<cv::Formatter> fmt = cv::Formatter::get(cv::Formatter::FMT_DEFAULT);
    // fmt->set64fPrecision(3);
    // fmt->set32fPrecision(3);
    // std::cout << fmt->format(kernel1.matrix()) << std::endl;

    // cv::Mat cv_image_filter1_view;
    // cv::normalize(cv_image_filter1, cv_image_filter1_view, 0, 255, cv::NORM_MINMAX);
    // cv::log(cv_image_filter1_view, cv_image_filter1_view);
    // cv::imshow("cv_image_filter1_view", cv_image_filter1_view);
    // cv::waitKey();

    // std::map<std::string, double> params2 = {{"r_in", 10}, {"r_out", 15}};
    // ohkl::RadialConvolutionKernel kernel2(params2);
    // cv::Mat cv_image_filter2;
    // cv::filter2D(cv_image_filter1, cv_image_filter2, -1, -kernel2.matrix(), anchor);

    // std::cout << fmt->format(kernel2.matrix()) << std::endl;

    // cv::Mat cv_image_filter2_view;
    // cv::normalize(cv_image_filter2, cv_image_filter2_view, 0, 255, cv::NORM_MINMAX);
    // cv::log(cv_image_filter2_view, cv_image_filter2_view);
    // cv::imshow("cv_image_filter2_view", cv_image_filter2_view);
    // cv::waitKey();


    // CHECK(false);
}
