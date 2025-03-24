//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/unit/qspace/TestQGrid2D.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/qspace/QGrid2D.h"

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"

#include <opencv2/core.hpp>
#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>

TEST_CASE("test/qspace/TestQGrid2D.cpp", "")
{
    const std::string filename = "Trypsin.ohkl";
    ohkl::Experiment experiment("Trypsin", "BioDiff");
    experiment.loadFromFile(filename);
    ohkl::sptrDataSet data = experiment.getData("Scan I");
    ohkl::sptrUnitCell cell = experiment.getSptrUnitCell("indexed");

    data->initBuffer(true);

    ohkl::QGrid2D grid(data, cell);
    grid.initGrid(ohkl::Miller::l, 3.0, 0.0005, 0.05);
    grid.sampleGrid();

    RealMatrix qgrid = grid.grid();
    cv::Mat qgrid_cv, qgrid_norm_cv, qgrid_norm_cv_not;
    cv::eigen2cv(qgrid, qgrid_cv);
    cv::normalize(qgrid_cv, qgrid_norm_cv, 1, 65534, cv::NORM_MINMAX, CV_16U);
    cv::bitwise_not(qgrid_norm_cv, qgrid_norm_cv_not);

    cv::imshow("q grid", qgrid_norm_cv_not);
    cv::waitKey();

    CHECK(false);
}
