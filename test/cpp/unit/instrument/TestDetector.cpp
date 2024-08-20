//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestDetector.cpp
//! @brief     Test Detector construction
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/detector/CylindricalDetector.h"
#include "test/cpp/catch.hpp"

#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"

TEST_CASE("test/instrument/TestDetector.cpp", "")
{
    const double eps = 1.0e-5;

    YAML::Node node = YAML::LoadFile("detector.yml");

    ohkl::CylindricalDetector detector(node["detector"]);

    CHECK(detector.gonio().nAxes() == 2);
    CHECK(detector.distance() == Approx(764.0 * ohkl::mm).epsilon(eps));
    CHECK(detector.angularWidth() == Approx(120.0 * ohkl::deg).epsilon(eps));
    CHECK(detector.height() == Approx(0.4).epsilon(eps));
    CHECK(detector.nCols() == 640);
    CHECK(detector.nRows() == 256);
    CHECK(detector.gain() == Approx(0.86).epsilon(eps));
    CHECK(detector.baseline() == Approx(0.0).epsilon(eps));
}
