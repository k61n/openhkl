//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestEnhancedAnnularImageFilter.cpp
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
#include "core/instrument/Diffractometer.h"
#include "core/loader/IDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/image/EnhancedAnnularImageFilter.h"

#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

    TEST_CASE("test/data/TestEnhancedAnnularImageFilter.cpp", "")
{
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

    std::map<std::string, double> params = {{"r1", 4.0}, {"r2", 8.0}, {"r3", 12.0}};
    ohkl::EnhancedAnnularImageFilter filter(params);
    filter.setImage(image);
    filter.filter();
    cv::Mat filtered_image = filter.cvFilteredImage();
    cv::Mat thresholded, thresholded_8u;

    cv::threshold(filtered_image, thresholded, 1.0, 255, cv::THRESH_BINARY_INV);
    thresholded.convertTo(thresholded_8u, CV_8U, 1.0);

    cv::SimpleBlobDetector::Params blob_params;
    blob_params.minThreshold = 1;
    blob_params.maxThreshold = 100;
    blob_params.filterByCircularity = false;
    blob_params.filterByConvexity = false;
    blob_params.filterByInertia = false;
    blob_params.filterByArea = false;
    blob_params.filterByColor = false;

    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(blob_params);
    detector->detect(thresholded_8u, keypoints);

    // cv::Mat with_keypoints, normalized_8u_not;
    // cv::drawKeypoints(thresholded_8u, keypoints, with_keypoints);

    // cv::imshow("With keypoints", with_keypoints);
    // cv::waitKey();

    CHECK(keypoints.size() == 476);
}
