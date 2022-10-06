//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/algo/TestPointsOnSphere.cpp
//! @brief     Not a real test: output points on sphere for visual inspection
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/algo/FFTIndexing.h"
#include <iostream>

// For the time being, we check the distribution of points by visual inspection.
// So this test is not a real test; it just exports the points which we then
// visualize with whatever external tool.
TEST_CASE("test/algo/TestPointsOnSphere.cpp", "")
{
    int nVertices = 10000;
    std::vector<Eigen::RowVector3d> points = ohkl::algo::pointsOnSphere(nVertices);
    for (const Eigen::RowVector3d& point : points)
        std::cout << point[0] << " " << point[1] << " " << point[2] << "\n";
}
