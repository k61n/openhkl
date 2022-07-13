//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/parser/TestParser.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/parser/Parser.h"
#include "test/cpp/catch.hpp"
#include <cstring>
#include <iostream>

TEST_CASE(__FILE__, "")
{
    // check parsing of ints
    std::vector<int> vecI;
    const char* pcI = "\n 0  1 \t 2  3  4\n5\t6    7 8   9   ";

    ohkl::readNumFromChar<int>(pcI, pcI + std::strlen(pcI), vecI);

    CHECK(vecI.size() == 10);
    for (int i = 0; i < vecI.size(); ++i)
        CHECK(vecI[i] == i);


    // check parsing of doubles
    std::vector<double> vecD;
    const char* pcD = "\n 0  1.1 \t 2.2  3.3  4.4\n5.5\t6.6    7.7 8.8   9.9   ";

    ohkl::readNumFromChar<double>(pcD, pcD + std::strlen(pcD), vecD);

    const double eps = 1e-8;
    CHECK(vecD.size() == 10);
    for (int i = 0; i < vecD.size(); ++i)
        CHECK(std::abs(vecD[i] - i - double(i) / 10.) <= eps);


    // check for empty string
    std::vector<int> vecI2;
    const char* pcI2 = "";

    ohkl::readNumFromChar<int>(pcI2, pcI2 + std::strlen(pcI2), vecI2);

    CHECK(vecI2.empty());
}
