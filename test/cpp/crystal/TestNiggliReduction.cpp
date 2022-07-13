//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestNiggliReduction.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <cmath>
#include <fstream>
#include <iostream>

#include <Eigen/Dense>

#include "base/utils/Units.h"
#include "tables/crystal/NiggliReduction.h"
#include "tables/crystal/UnitCell.h"

const double tolerance = 1e-6;

TEST_CASE("test/crystal/TestNiggliReduction.cpp", "")
{
    const unsigned int nCells = 783;

    std::ifstream primitiveCellsFile("lattices.dat");
    std::ifstream niggliCellsFile("niggli_lattices.dat");

    std::string line;

    for (unsigned int i = 0; i < 4; ++i) {
        std::getline(primitiveCellsFile, line);
        std::getline(niggliCellsFile, line);
    }

    std::istringstream iss;
    for (unsigned int i = 0; i < nCells; ++i) {
        std::getline(primitiveCellsFile, line);
        std::getline(niggliCellsFile, line);

        std::getline(primitiveCellsFile, line);
        iss.str(line);
        Eigen::Vector3d primitive_a;
        iss >> primitive_a[0] >> primitive_a[1] >> primitive_a[2];
        iss.clear();

        std::getline(niggliCellsFile, line);
        iss.str(line);
        Eigen::Vector3d niggli_a;
        iss >> niggli_a[0] >> niggli_a[1] >> niggli_a[2];
        iss.clear();

        std::getline(primitiveCellsFile, line);
        iss.str(line);
        Eigen::Vector3d primitive_b;
        iss >> primitive_b[0] >> primitive_b[1] >> primitive_b[2];
        iss.clear();

        std::getline(niggliCellsFile, line);
        iss.str(line);
        Eigen::Vector3d niggli_b;
        iss >> niggli_b[0] >> niggli_b[1] >> niggli_b[2];
        iss.clear();

        std::getline(primitiveCellsFile, line);
        iss.str(line);
        Eigen::Vector3d primitive_c;
        iss >> primitive_c[0] >> primitive_c[1] >> primitive_c[2];
        iss.clear();

        std::getline(niggliCellsFile, line);
        iss.str(line);
        Eigen::Vector3d niggli_c;
        iss >> niggli_c[0] >> niggli_c[1] >> niggli_c[2];
        iss.clear();

        Eigen::Matrix3d basis;
        basis.col(0) = primitive_a;
        basis.col(1) = primitive_b;
        basis.col(2) = primitive_c;
        ohkl::UnitCell uc(basis);
        ohkl::NiggliReduction niggli_reducer(uc.metric(), 1.0e-5);
        Eigen::Matrix3d newg, P;
        niggli_reducer.reduce(newg, P);
        std::cout << i << std::endl;
        uc.transform(P);

        Eigen::Vector3d calc_niggli_a = uc.basis().col(0);
        CHECK(calc_niggli_a[0] == Approx(niggli_a[0]).epsilon(tolerance));
        CHECK(calc_niggli_a[1] == Approx(niggli_a[1]).epsilon(tolerance));
        CHECK(calc_niggli_a[2] == Approx(niggli_a[2]).epsilon(tolerance));

        Eigen::Vector3d calc_niggli_b = uc.basis().col(1);
        CHECK(calc_niggli_b[0] == Approx(niggli_b[0]).epsilon(tolerance));
        CHECK(calc_niggli_b[1] == Approx(niggli_b[1]).epsilon(tolerance));
        CHECK(calc_niggli_b[2] == Approx(niggli_b[2]).epsilon(tolerance));

        Eigen::Vector3d calc_niggli_c = uc.basis().col(2);
        CHECK(calc_niggli_c[0] == Approx(niggli_c[0]).epsilon(tolerance));
        CHECK(calc_niggli_c[1] == Approx(niggli_c[1]).epsilon(tolerance));
        CHECK(calc_niggli_c[2] == Approx(niggli_c[2]).epsilon(tolerance));
    }

    primitiveCellsFile.close();
    niggliCellsFile.close();
}
