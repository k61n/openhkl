//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestSample.cpp
//! @brief     Test Sample construction
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
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Sample.h"

TEST_CASE("test/instrument/TestSample.cpp", "")
{
    YAML::Node node = YAML::LoadFile("sample.yml");

    ohkl::Sample sample(node["sample"]);

    CHECK(sample.gonio().nAxes() == 3);
}
