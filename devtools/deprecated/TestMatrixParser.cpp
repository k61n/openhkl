//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/utils/TestMatrixParser.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <string>

#include <Eigen/Dense>

#include "core/detector/DataOrder.h"
#include "core/detector/MatrixParser.h"

TEST_CASE("test/utils/TestMatrixParser.cpp", "")
{
    Eigen::MatrixXi m;
    m.resize(3, 4);
    std::string s("1 2 3 4 5 6 7 8 9 10 11 12");

    nsx::MatrixParser parser;
    parser(nsx::DataOrder::TopLeftColMajor, s, m);

    CHECK(m(0, 0) == 3);
    CHECK(m(0, 1) == 6);
    CHECK(m(0, 2) == 9);
    CHECK(m(0, 3) == 12);
    CHECK(m(1, 0) == 2);
    CHECK(m(1, 1) == 5);
    CHECK(m(1, 2) == 8);
    CHECK(m(1, 3) == 11);
    CHECK(m(2, 0) == 1);
    CHECK(m(2, 1) == 4);
    CHECK(m(2, 2) == 7);
    CHECK(m(2, 3) == 10);

    parser(nsx::DataOrder::TopLeftRowMajor, s, m);
    CHECK(m(0, 0) == 9);
    CHECK(m(0, 1) == 10);
    CHECK(m(0, 2) == 11);
    CHECK(m(0, 3) == 12);
    CHECK(m(1, 0) == 5);
    CHECK(m(1, 1) == 6);
    CHECK(m(1, 2) == 7);
    CHECK(m(1, 3) == 8);
    CHECK(m(2, 0) == 1);
    CHECK(m(2, 1) == 2);
    CHECK(m(2, 2) == 3);
    CHECK(m(2, 3) == 4);

    parser(nsx::DataOrder::TopRightColMajor, s, m);
    CHECK(m(0, 0) == 12);
    CHECK(m(0, 1) == 9);
    CHECK(m(0, 2) == 6);
    CHECK(m(0, 3) == 3);
    CHECK(m(1, 0) == 11);
    CHECK(m(1, 1) == 8);
    CHECK(m(1, 2) == 5);
    CHECK(m(1, 3) == 2);
    CHECK(m(2, 0) == 10);
    CHECK(m(2, 1) == 7);
    CHECK(m(2, 2) == 4);
    CHECK(m(2, 3) == 1);

    parser(nsx::DataOrder::TopRightRowMajor, s, m);
    CHECK(m(0, 0) == 12);
    CHECK(m(0, 1) == 11);
    CHECK(m(0, 2) == 10);
    CHECK(m(0, 3) == 9);
    CHECK(m(1, 0) == 8);
    CHECK(m(1, 1) == 7);
    CHECK(m(1, 2) == 6);
    CHECK(m(1, 3) == 5);
    CHECK(m(2, 0) == 4);
    CHECK(m(2, 1) == 3);
    CHECK(m(2, 2) == 2);
    CHECK(m(2, 3) == 1);

    parser(nsx::DataOrder::BottomLeftColMajor, s, m);
    CHECK(m(0, 0) == 1);
    CHECK(m(0, 1) == 4);
    CHECK(m(0, 2) == 7);
    CHECK(m(0, 3) == 10);
    CHECK(m(1, 0) == 2);
    CHECK(m(1, 1) == 5);
    CHECK(m(1, 2) == 8);
    CHECK(m(1, 3) == 11);
    CHECK(m(2, 0) == 3);
    CHECK(m(2, 1) == 6);
    CHECK(m(2, 2) == 9);
    CHECK(m(2, 3) == 12);

    parser(nsx::DataOrder::BottomLeftRowMajor, s, m);
    CHECK(m(0, 0) == 1);
    CHECK(m(0, 1) == 2);
    CHECK(m(0, 2) == 3);
    CHECK(m(0, 3) == 4);
    CHECK(m(1, 0) == 5);
    CHECK(m(1, 1) == 6);
    CHECK(m(1, 2) == 7);
    CHECK(m(1, 3) == 8);
    CHECK(m(2, 0) == 9);
    CHECK(m(2, 1) == 10);
    CHECK(m(2, 2) == 11);
    CHECK(m(2, 3) == 12);

    parser(nsx::DataOrder::BottomRightColMajor, s, m);
    CHECK(m(0, 0) == 10);
    CHECK(m(0, 1) == 7);
    CHECK(m(0, 2) == 4);
    CHECK(m(0, 3) == 1);
    CHECK(m(1, 0) == 11);
    CHECK(m(1, 1) == 8);
    CHECK(m(1, 2) == 5);
    CHECK(m(1, 3) == 2);
    CHECK(m(2, 0) == 12);
    CHECK(m(2, 1) == 9);
    CHECK(m(2, 2) == 6);
    CHECK(m(2, 3) == 3);

    parser(nsx::DataOrder::BottomRightRowMajor, s, m);
    CHECK(m(0, 0) == 4);
    CHECK(m(0, 1) == 3);
    CHECK(m(0, 2) == 2);
    CHECK(m(0, 3) == 1);
    CHECK(m(1, 0) == 8);
    CHECK(m(1, 1) == 7);
    CHECK(m(1, 2) == 6);
    CHECK(m(1, 3) == 5);
    CHECK(m(2, 0) == 12);
    CHECK(m(2, 1) == 11);
    CHECK(m(2, 2) == 10);
    CHECK(m(2, 3) == 9);
}
