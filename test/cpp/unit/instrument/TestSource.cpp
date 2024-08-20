//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestSource.cpp
//! @brief     Test Source construction
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/utils/Units.h"
#include "base/utils/YAMLType.h"
#include "core/instrument/Source.h"

TEST_CASE("test/instrument/TestSource.cpp", "")
{
    const double eps = 1.0e-5;

    YAML::Node node = YAML::LoadFile("source.yml");

    ohkl::Source source(node["source"]);

    CHECK(source.nMonochromators() == 1);
    CHECK(source.monochromators()[0].width() == Approx(1 * ohkl::mm).epsilon(eps));
    CHECK(source.monochromators()[0].wavelength() == Approx(2.6734).epsilon(eps));
    CHECK(source.monochromators()[0].fullWidthHalfMaximum() == Approx(0.00901).epsilon(eps));
}
