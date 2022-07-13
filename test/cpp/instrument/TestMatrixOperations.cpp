//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/instrument/TestMatrixOperations.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/MatrixOperations.h"
#include "test/cpp/catch.hpp"

/*
 * test matrix element removal
 */
void test_element_removal()
{
    int k = 0;
    Eigen::MatrixXd M(3, 3);
    for (int i = 0; i < M.rows(); ++i)
        for (int j = 0; j < M.cols(); ++j)
            M(i, j) = k++;

    Eigen::MatrixXd M1 = M;
    Eigen::MatrixXd M2 = M;
    ohkl::removeColumn(M1, 1);
    ohkl::removeRow(M2, 1);


    const double eps = 1e-6;

    CHECK(std::abs(M1(0, 0) - M(0, 0)) <= eps);
    CHECK(std::abs(M1(0, 1) - M(0, 2)) <= eps);
    CHECK(std::abs(M1(1, 0) - M(1, 0)) <= eps);
    CHECK(std::abs(M1(1, 1) - M(1, 2)) <= eps);

    CHECK(std::abs(M2(0, 0) - M(0, 0)) <= eps);
    CHECK(std::abs(M2(0, 1) - M(0, 1)) <= eps);
    CHECK(std::abs(M2(1, 0) - M(2, 0)) <= eps);
    CHECK(std::abs(M2(1, 1) - M(2, 1)) <= eps);
}


/*
 * test by comparing with direct calculation
 */
void test_interpolation()
{
    Eigen::Matrix3d mat1;
    mat1(0, 0) = 1.0000;
    mat1(0, 1) = 0.0000;
    mat1(0, 2) = 0.0000;
    mat1(1, 0) = 0.0000;
    mat1(1, 1) = 0.7071;
    mat1(1, 2) = -0.7071;
    mat1(2, 0) = 0.0000;
    mat1(2, 1) = 0.7071;
    mat1(2, 2) = 0.7071;

    Eigen::Matrix3d mat2;
    mat2(0, 0) = 0.4067;
    mat2(0, 1) = 0.0000;
    mat2(0, 2) = 0.9136;
    mat2(1, 0) = 0.0000;
    mat2(1, 1) = 1.0000;
    mat2(1, 2) = 0.0000;
    mat2(2, 0) = -0.9136;
    mat2(2, 1) = 0.0000;
    mat2(2, 2) = 0.4067;

    Eigen::Matrix3d mat = ohkl::interpolateRotation(mat1, mat2, 0.25);


    const double eps = 1e-4;

    CHECK(std::abs(mat(0, 0) - 0.9570) <= eps);
    CHECK(std::abs(mat(0, 1) - 0.0872) <= eps);
    CHECK(std::abs(mat(0, 2) - 0.2768) <= eps);

    CHECK(std::abs(mat(1, 0) - 0.0872) <= eps);
    CHECK(std::abs(mat(1, 1) - 0.8233) <= eps);
    CHECK(std::abs(mat(1, 2) + 0.5609) <= eps);

    CHECK(std::abs(mat(2, 0) + 0.2768) <= eps);
    CHECK(std::abs(mat(2, 1) - 0.5609) <= eps);
    CHECK(std::abs(mat(2, 2) - 0.7803) <= eps);
}


TEST_CASE(__FILE__, "")
{
    test_element_removal();
    test_interpolation();
}
