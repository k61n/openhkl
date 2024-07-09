//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestGonio.cpp
//! @brief     Test Goniometer construction
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/geometry/DirectVector.h"
#include "test/cpp/catch.hpp"

#include "base/utils/YAMLType.h"
#include "core/gonio/Axis.h"
#include "core/gonio/Gonio.h"

TEST_CASE("test/instrument/TestGonio.cpp", "")
{
    const double eps = 1.0e-5;

    YAML::Node node = YAML::LoadFile("gonio.yml");

    ohkl::Gonio gonio(node["goniometer"]);

    CHECK(gonio.nAxes() == 3);

    std::vector<double> id_state = {0, 0, 0};
    std::vector<double> state = {30, 60, 90};

    Eigen::Transform<double, 3, Eigen::Affine> id_trans = gonio.affineMatrix(id_state);
    Eigen::Transform<double, 3, Eigen::Affine> trans = gonio.affineMatrix(state);

    CHECK(id_trans.matrix()(0, 0) == Approx(1).epsilon(eps));
    CHECK(id_trans.matrix()(1, 0) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(2, 0) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(3, 0) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(0, 1) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(1, 1) == Approx(1).epsilon(eps));
    CHECK(id_trans.matrix()(2, 1) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(3, 1) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(0, 2) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(1, 2) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(2, 2) == Approx(1).epsilon(eps));
    CHECK(id_trans.matrix()(3, 2) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(0, 3) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(1, 3) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(2, 3) == Approx(0).epsilon(eps));
    CHECK(id_trans.matrix()(3, 3) == Approx(1).epsilon(eps));

    CHECK(trans.matrix()(0, 0) == Approx(-0.146911).epsilon(eps));
    CHECK(trans.matrix()(1, 0) == Approx(-0.941014).epsilon(eps));
    CHECK(trans.matrix()(2, 0) == Approx(0.304811).epsilon(eps));
    CHECK(trans.matrix()(3, 0) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 1) == Approx(0.484744).epsilon(eps));
    CHECK(trans.matrix()(1, 1) == Approx(0.200122 ).epsilon(eps));
    CHECK(trans.matrix()(2, 1) == Approx(0.851454).epsilon(eps));
    CHECK(trans.matrix()(3, 1) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 2) == Approx(-0.86223).epsilon(eps));
    CHECK(trans.matrix()(1, 2) == Approx(0.272843).epsilon(eps));
    CHECK(trans.matrix()(2, 2) == Approx(0.426751).epsilon(eps));
    CHECK(trans.matrix()(3, 2) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(1, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(2, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(3, 3) == Approx(1).epsilon(eps));

    ohkl::DirectVector vec1(0, 0, 1);
    ohkl::DirectVector vec2(1, 2, 3);

    ohkl::DirectVector id_trans_vec1 = gonio.transform(vec1, id_state);
    ohkl::DirectVector trans_vec1 = gonio.transform(vec1, state);
    ohkl::DirectVector id_trans_vec2 = gonio.transform(vec2, id_state);
    ohkl::DirectVector trans_vec2 = gonio.transform(vec2, state);

    CHECK(id_trans_vec1[0] == Approx(0).epsilon(eps));
    CHECK(id_trans_vec1[1] == Approx(0).epsilon(eps));
    CHECK(id_trans_vec1[2] == Approx(1).epsilon(eps));

    CHECK(trans_vec1[0] == Approx(-0.8622296831).epsilon(eps));
    CHECK(trans_vec1[1] == Approx(0.2728432668).epsilon(eps));
    CHECK(trans_vec1[2] == Approx(0.4267511282).epsilon(eps));

    CHECK(id_trans_vec2[0] == Approx(1).epsilon(eps));
    CHECK(id_trans_vec2[1] == Approx(2).epsilon(eps));
    CHECK(id_trans_vec2[2] == Approx(3).epsilon(eps));

    CHECK(trans_vec2[0] == Approx(-1.7641113861).epsilon(eps));
    CHECK(trans_vec2[1] == Approx(0.277760246).epsilon(eps));
    CHECK(trans_vec2[2] == Approx(3.2879720594).epsilon(eps));
}
