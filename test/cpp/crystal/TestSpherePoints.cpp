//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/crystal/TestFFTIndexingSphere.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <fstream>
#include <iostream>

#include "core/algo/FFTIndexing.h"


TEST_CASE("test/crystal/TestSpherePoints.cpp", "")
{
    unsigned int n_vertices = 128;
    std::vector<Eigen::RowVector3d> points = ohkl::algo::pointsOnSphere(n_vertices);

    // plot these points with:
    // gnuplot -p -e "set xyplane 0; splot \"sphere.dat\" u 1:2:3 w points pt 7
    std::ofstream ofstrdat("sphere.dat");

    for (const Eigen::RowVector3d& vec : points) {
        ofstrdat << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
    }

    CHECK(points.size() == n_vertices);
}
