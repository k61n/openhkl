//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/mathematics/TestRandom.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/Random.h"
#include "test/cpp/catch.hpp"
#include <iostream>


TEST_CASE(__FILE__, "")
{
    try {
        double d1 = nsx::Random::double01();
        double d2 = nsx::Random::double01();

        double d3 = nsx::Random::doubleRange(0, 1000);
        double d4 = nsx::Random::doubleRange(0, 1000);

        CHECK(d1 != d2);
        CHECK(d3 != d4);


        int i1 = nsx::Random::intRange();
        int i2 = nsx::Random::intRange();

        CHECK(i1 != i2);
    } catch (std::exception& e) {
        FAIL(std::string("caught exception: ") + e.what());
    } catch (...) {
        FAIL("unknown exception while loading data");
    }
}
