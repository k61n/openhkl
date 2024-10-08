//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/geometry/TestProfile1D.cpp
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
#include "core/shape/Profile1D.h"

#include <Eigen/Dense>

TEST_CASE("test/geometry/TestProfile1D.cpp", "")
{
    double eps = 1.0e-5;

    ohkl::Intensity bkg(10.0, 0.1);
    ohkl::Profile1D profile(bkg, 3.0, 3);

    profile.addPoint(0.5, 10);
    profile.addPoint(5.0, 20);
    profile.addPoint(7.5, 30);

    CHECK(profile.profile().size() == 3);
    CHECK(profile.npoints()[0] == 1);
    CHECK(profile.npoints()[1] == 2);
    CHECK(profile.npoints()[2] == 3);
    CHECK(profile.counts()[0] == Approx(10).epsilon(eps));
    CHECK(profile.counts()[1] == Approx(30).epsilon(eps));
    CHECK(profile.counts()[2] == Approx(60).epsilon(eps));
    CHECK(profile.profile()[0].value() == Approx(0.0).epsilon(eps));
    CHECK(profile.profile()[1].value() == Approx(0.33333333).epsilon(eps));
    CHECK(profile.profile()[2].value() == Approx(1.0).epsilon(eps));
}
