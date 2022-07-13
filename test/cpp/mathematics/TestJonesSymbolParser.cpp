//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/mathematics/TestJonesSymbolParser.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "tables/crystal/JonesSymbolParser.h"
#include <iostream>

const double tolerance = 1e-6;

TEST_CASE("test/mathematics/TestJonesSymbolParser.cpp", "")
{
    Eigen::Transform<double, 3, Eigen::Affine> matrix;

    CHECK_NOTHROW(matrix = ohkl::parseJonesSymbol(" x+4y-z+1/2,-x + y -3z +2,-x-y-z +3\t"));

    // Compare
    CHECK(matrix(0, 0) == Approx(1.0).epsilon(tolerance));
    CHECK(matrix(0, 1) == Approx(4.0).epsilon(tolerance));
    CHECK(matrix(0, 2) == Approx(-1.0).epsilon(tolerance));
    CHECK(matrix(0, 3) == Approx(0.5).epsilon(tolerance));
    CHECK(matrix(1, 0) == Approx(-1.0).epsilon(tolerance));
    CHECK(matrix(1, 1) == Approx(1.0).epsilon(tolerance));
    CHECK(matrix(1, 2) == Approx(-3.0).epsilon(tolerance));
    CHECK(matrix(1, 3) == Approx(2.0).epsilon(tolerance));
    CHECK(matrix(2, 0) == Approx(-1.0).epsilon(tolerance));
    CHECK(matrix(2, 1) == Approx(-1.0).epsilon(tolerance));
    CHECK(matrix(2, 2) == Approx(-1.0).epsilon(tolerance));
    CHECK(matrix(2, 3) == Approx(3.0).epsilon(tolerance));
    CHECK(std::abs(matrix(3, 0)) < tolerance);
    CHECK(std::abs(matrix(3, 1)) < tolerance);
    CHECK(std::abs(matrix(3, 2)) < tolerance);
    CHECK(matrix(3, 3) == Approx(1.0).epsilon(tolerance));

    CHECK_NOTHROW(matrix = ohkl::parseJonesSymbol("x,y,z"));

    // Compare
    CHECK(matrix(0, 0) == Approx(1.0).epsilon(tolerance));
    CHECK(std::abs(matrix(0, 1)) < tolerance);
    CHECK(std::abs(matrix(0, 2)) < tolerance);
    CHECK(std::abs(matrix(0, 3)) < tolerance);
    CHECK(std::abs(matrix(1, 0)) < tolerance);
    CHECK(matrix(1, 1) == Approx(1.0).epsilon(tolerance));
    CHECK(std::abs(matrix(1, 2)) < tolerance);
    CHECK(std::abs(matrix(1, 3)) < tolerance);
    CHECK(std::abs(matrix(2, 0)) < tolerance);
    CHECK(std::abs(matrix(2, 1)) < tolerance);
    CHECK(matrix(2, 2) == Approx(1.0).epsilon(tolerance));
    CHECK(std::abs(matrix(2, 3)) < tolerance);
    CHECK(std::abs(matrix(3, 0)) < tolerance);
    CHECK(std::abs(matrix(3, 1)) < tolerance);
    CHECK(std::abs(matrix(3, 2)) < tolerance);
    CHECK(matrix(3, 3) == Approx(1.0).epsilon(tolerance));

    CHECK_THROWS(matrix = ohkl::parseJonesSymbol("2x"));

    CHECK_THROWS(matrix = ohkl::parseJonesSymbol("2x,4y"));

    CHECK_THROWS(matrix = ohkl::parseJonesSymbol("1,1,1"));

    CHECK_THROWS(matrix = ohkl::parseJonesSymbol("2x,ay+3z,$z+1/2"));
}
