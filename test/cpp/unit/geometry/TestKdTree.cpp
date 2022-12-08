//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestKdTree.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/geometry/KdTree.h"

#include <iostream>

TEST_CASE("test/geometry/TestKdTree.cpp", "")
{
    typedef ohkl::Point<int, 2> point2d;
    typedef ohkl::KdTree<int, 2> tree2d;

    point2d points[] = {{2, 3}, {5, 4}, {9, 6}, {4, 7}, {8, 1}, {7, 2}};

    tree2d tree(std::begin(points), std::end(points));
    point2d n = tree.nearest({9, 2});

    std::cout << "nearest point: " << n << '\n';
    std::cout << "distance: " << tree.distance() << '\n';
    std::cout << "nodes visited: " << tree.visited() << '\n';
    CHECK(n.get(0) == 8);
    CHECK(n.get(1) == 1);
    CHECK(std::fabs(tree.distance() - 1.41421) < 0.001);
    CHECK(tree.visited() == 3);
}
