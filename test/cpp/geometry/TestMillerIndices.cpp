//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/geometry/TestMillerIndices.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <Eigen/Dense>

#include "tables/crystal/MillerIndex.h"

TEST_CASE("test/geometry/TestMillerIndices.cpp", "")
{
    Eigen::RowVector3i v1(1, 2, 3);

    ohkl::MillerIndex hkl1(v1);

    int& x = hkl1[0];

    x = 100;

    CHECK(hkl1[0] == 100);
}
