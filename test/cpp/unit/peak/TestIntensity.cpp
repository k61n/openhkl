//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/integrate/TestIntensity.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "core/peak/Intensity.h"

#include <iostream>

TEST_CASE("test/integrate/TestIntensity.cpp", "")
{
    const double eps = 1.0e-5;

    const ohkl::Intensity intensity1(1000, 10);
    const ohkl::Intensity intensity2(2500, 30);

    CHECK(intensity1.value() == Approx(1000).epsilon(eps));
    CHECK(intensity2.sigma() == Approx(std::sqrt(30)).epsilon(eps));
    CHECK(intensity1.strength() ==
          Approx(intensity1.value() / intensity1.sigma()).epsilon(eps));
    CHECK(intensity1.variance() == Approx(10).epsilon(eps));

    const ohkl::Intensity sum = intensity1 + intensity2;
    CHECK(sum.value() == Approx(3500).epsilon(eps));
    CHECK(sum.sigma() == Approx(std::sqrt(40)).epsilon(eps));

    const ohkl::Intensity diff = intensity2 - intensity1;
    CHECK(diff.value() == Approx(1500).epsilon(eps));
    CHECK(diff.sigma() == Approx(std::sqrt(40)).epsilon(eps));

    const ohkl::Intensity product = intensity1 * 3.0;
    CHECK(product.value() == Approx(3000).epsilon(eps));
    CHECK(product.sigma() == Approx(std::sqrt(90)).epsilon(eps));

    const ohkl::Intensity quotient = intensity2 / intensity1;
    CHECK(quotient.value() == Approx(2.5).epsilon(eps));
    CHECK(quotient.variance() == Approx(0.02503).epsilon(eps));
}
