//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestFFTIndexingSphere.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <iostream>
#include <fstream>

#include "core/algo/FFTIndexing.h"


TEST_CASE("test/crystal/TestFFTIndexingSphere.cpp", "")
{
    unsigned int n_vertices = 128;
    std::vector<Eigen::RowVector3d> points = nsx::algo::pointsOnSphere(n_vertices);

    // plot these points with: gnuplot -p -e "set xyplane 0; splot \"sphere.dat\" u 1:2:3 w points pt 7"
    std::ofstream ofstrdat("sphere.dat");

    for(const Eigen::RowVector3d& vec : points)
    {
        ofstrdat << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
    }

    CHECK(points.size() == n_vertices);
}
