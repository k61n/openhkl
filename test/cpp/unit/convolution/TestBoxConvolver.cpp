//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestBoxConvolver.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/convolve/ConstantConvolver.h"
#include "core/convolve/Convolver.h"
#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/PeakFinder2D.h"
#include "core/instrument/Diffractometer.h"
#include "core/loader/RawDataReader.h"

#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

TEST_CASE("test/data/TestBoxConvolver.cpp", "")
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
    data->addRawFrame(filename);
    data->finishRead();

    ohkl::RealMatrix image = data->transformedFrame(0);

    std::map<std::string, double> convolver_params = {{"box_size", 3}};
    ohkl::ConstantConvolver convolver(convolver_params);
    ohkl::RealMatrix filtered_image = convolver.convolve(image);
    double maxval = filtered_image.maxCoeff();
    double minval = filtered_image.minCoeff();
    CHECK(maxval == Approx(2101.1111111111).epsilon(eps));
    CHECK(minval == Approx(-0.2380952381).epsilon(eps));

    double threshold = 150;
    cv::Mat cv_filtered_image, cv_thresholded_image, cv_image_8u;
    int max_bin_val = pow(2, 16);
    cv::eigen2cv(filtered_image, cv_filtered_image);
    cv::threshold(
        cv_filtered_image, cv_thresholded_image, threshold, max_bin_val, cv::THRESH_BINARY_INV);
    cv_thresholded_image.convertTo(cv_image_8u, CV_8U, 1.0);

    cv::imwrite("box.jpg", cv_image_8u);

    cv::SimpleBlobDetector::Params params;
    params.minThreshold = 1;
    params.maxThreshold = 100;
    params.filterByCircularity = false;
    params.filterByConvexity = false;
    params.filterByInertia = false;
    params.filterByArea = false;
    params.filterByColor = false;

    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
    detector->detect(cv_image_8u, keypoints);
    CHECK(keypoints.size() == 97);
}
