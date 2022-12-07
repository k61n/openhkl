//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/data/TestXFileReader.cpp
//! @brief     Tests the DENZO .x file reader
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <iostream>

#include "core/loader/XFileHandler.h"

TEST_CASE("test/data/TestXFileReader.cpp", "")
{
    std::string xfile_name = "soak_9_d2_I_scanI_9483.x";

    int n_peaks = 2141;
    double x_0 = 1.6;
    double y_0 = 1010.3;
    double x_2140 = 897.4;
    double y_2140 = 1490.1;
    double eps = 1.0e-6;

    ohkl::XFileHandler xfh(xfile_name);
    xfh.readXFile(0);
    std::vector<Eigen::Vector3d> peaks = xfh.getPeakCenters();

    CHECK(peaks.size() == n_peaks);
    CHECK(peaks[0][1] - x_0 < eps);
    CHECK(peaks[0][0] - y_0 < eps);
    CHECK(peaks[2140][1] - x_2140 < eps);
    CHECK(peaks[2140][0] - y_2140 < eps);
}
