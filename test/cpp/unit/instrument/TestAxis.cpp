//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/instrument/TestAxis.cpp
//! @brief     Test Axis construction
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "base/utils/Units.h"
#include "core/gonio/Axis.h"
#include "core/gonio/RotAxis.h"

TEST_CASE("test/instrument/TestAxis.cpp", "")
{
    const double eps = 1.0e-5;

    YAML::Node node = YAML::LoadFile("axis.yml");

    ohkl::Axis* axis = ohkl::Axis::create(node["axis"]);

    CHECK(dynamic_cast<ohkl::RotAxis*>(axis) != nullptr);

    ohkl::RotAxis* rotaxis = dynamic_cast<ohkl::RotAxis*>(axis);

    Eigen::Matrix3d id_mat = rotaxis->rotationMatrix(0.0);
    CHECK(id_mat(0, 0) == Approx(1).epsilon(eps));
    CHECK(id_mat(1, 1) == Approx(1).epsilon(eps));
    CHECK(id_mat(2, 2) == Approx(1).epsilon(eps));
    CHECK(id_mat(0, 1) == Approx(0).epsilon(eps));
    CHECK(id_mat(0, 2) == Approx(0).epsilon(eps));
    CHECK(id_mat(1, 0) == Approx(0).epsilon(eps));
    CHECK(id_mat(1, 2) == Approx(0).epsilon(eps));
    CHECK(id_mat(2, 0) == Approx(0).epsilon(eps));
    CHECK(id_mat(2, 1) == Approx(0).epsilon(eps));

    Eigen::Matrix3d mat = rotaxis->rotationMatrix(90.0);
    CHECK(mat(0, 0) == Approx(-0.448074).epsilon(eps));
    CHECK(mat(1, 1) == Approx(-0.448074).epsilon(eps));
    CHECK(mat(2, 2) == Approx(1).epsilon(eps));
    CHECK(mat(0, 1) == Approx(0.893997).epsilon(eps));
    CHECK(mat(1, 0) == Approx(-0.893997).epsilon(eps));
    CHECK(mat(0, 2) == Approx(0).epsilon(eps));
    CHECK(mat(1, 2) == Approx(0).epsilon(eps));
    CHECK(mat(2, 0) == Approx(0).epsilon(eps));
    CHECK(mat(2, 1) == Approx(0).epsilon(eps));

    Eigen::Quaterniond id_quat = rotaxis->quaternion(0.0);
    CHECK(id_quat.coeffs()[0] == Approx(0).epsilon(eps));
    CHECK(id_quat.coeffs()[1] == Approx(0).epsilon(eps));
    CHECK(id_quat.coeffs()[2] == Approx(0).epsilon(eps));
    CHECK(id_quat.coeffs()[3] == Approx(1).epsilon(eps));

    Eigen::Quaterniond quat = rotaxis->quaternion(90.0);
    CHECK(quat.coeffs()[0] == Approx(0).epsilon(eps));
    CHECK(quat.coeffs()[1] == Approx(0).epsilon(eps));
    CHECK(quat.coeffs()[2] == Approx(-0.850904).epsilon(eps));
    CHECK(quat.coeffs()[3] == Approx(0.525322).epsilon(eps));

    Eigen::Transform<double, 3, Eigen::Affine> id_trans = rotaxis->affineMatrix(0.0);
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

    Eigen::Transform<double, 3, Eigen::Affine> trans = rotaxis->affineMatrix(90.0);
    CHECK(trans.matrix()(0, 0) == Approx(-0.448074).epsilon(eps));
    CHECK(trans.matrix()(1, 0) == Approx(-0.893997).epsilon(eps));
    CHECK(trans.matrix()(2, 0) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(3, 0) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 1) == Approx(0.893997).epsilon(eps));
    CHECK(trans.matrix()(1, 1) == Approx(-0.448074).epsilon(eps));
    CHECK(trans.matrix()(2, 1) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(3, 1) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 2) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(1, 2) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(2, 2) == Approx(1).epsilon(eps));
    CHECK(trans.matrix()(3, 2) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(0, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(1, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(2, 3) == Approx(0).epsilon(eps));
    CHECK(trans.matrix()(3, 3) == Approx(1).epsilon(eps));
}
