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

#include "core/data/SingleFrame.h"
#include "core/experiment/Experiment.h"
#include "core/image/AnnularImageFilter.h"
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

    ohkl::FilterParameters params = {{"r1", 4.0}, {"r2", 8.0}, {"r3", 12.0}};
    ohkl::AnnularImageFilter filter(params);
    filter.setImage(image);
    filter.filter();

    // cv::Mat cv_image_view;
    // cv::normalize(cv_image, cv_image_view, 0, 65535, cv::NORM_MINMAX);
    // cv::log(cv_image_view, cv_image_view);
    // cv::imshow("cv_image_view", cv_image_view);

    CHECK(false);
}
