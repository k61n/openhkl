//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestDiffractometer.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Monochromator.h"
#include "test/cpp/catch.hpp"

#include "core/instrument/Diffractometer.h"

const double eps = 1.0e-5;

TEST_CASE("test/instrument/TestDiffractometer.cpp", "")
{

    ohkl::Diffractometer* diffractometer = ohkl::Diffractometer::create("BioDiff");
    CHECK(diffractometer->name() == "BioDiff");


    ohkl::Detector* detector = diffractometer->detector();
    CHECK(detector->name() == "banana");
    CHECK(detector->distance() == Approx(0.199).epsilon(eps));
    CHECK(detector->angularWidth() == Approx(360.0 * ohkl::deg).epsilon(eps));
    CHECK(detector->height() == Approx(0.44824 * ohkl::m).epsilon(eps));
    CHECK(detector->baseline() == Approx(227.0).epsilon(eps));
    CHECK(detector->gain() == Approx(7.0).epsilon(eps));

    ohkl::Gonio& detector_gonio = detector->gonio();
    CHECK(detector_gonio.nAxes() == 1);
    ohkl::Axis& detector_axis = detector_gonio.axis(0);
    CHECK(detector_axis.name() == "2theta(gamma)");
    CHECK(detector_axis.axis()[2] == Approx(1.0).epsilon(eps)); // (0 0 1) axis


    ohkl::Sample& sample = diffractometer->sample();

    ohkl::Gonio& sample_gonio = sample.gonio();
    CHECK(sample_gonio.nAxes() == 1);
    ohkl::Axis& sample_axis = sample_gonio.axis(0);
    CHECK(sample_axis.name() == "omega");
    CHECK(sample_axis.axis()[2] == Approx(1.0).epsilon(eps)); // (0 0 1) axis

    ohkl::Source& source = diffractometer->source();
    CHECK(source.nMonochromators() == 1);
    ohkl::Monochromator& mono = source.selectedMonochromator();
    CHECK(mono.wavelength() == Approx(2.6734).epsilon(eps));
    CHECK(mono.width() == Approx(1.0 * ohkl::mm).epsilon(eps));
    CHECK(mono.height() == Approx(1.0 * ohkl::mm).epsilon(eps));
}
