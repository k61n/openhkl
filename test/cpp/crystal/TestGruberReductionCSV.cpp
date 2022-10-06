//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      test/cpp/crystal/TestGruberReductionCSV.cpp
//! @brief     Test ...
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "base/utils/CSV.h"
#include "base/utils/Units.h"
#include "core/gonio/Component.h"
#include "tables/crystal/GruberReduction.h"
#include "tables/crystal/NiggliReduction.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;

TEST_CASE("test/crystal/TestGruberReductionCSV.cpp", "")
{
    // using vectord = vector<double>;
    std::ofstream outfile;

    outfile.open("output.tsv", std::fstream::out);

    std::ifstream database;
    database.open("crystallography.tsv", std::fstream::in);
    ohkl::CSV csv_reader('\t', '#');

    CHECK(database.is_open());

    unsigned int total, correct;

    total = 0;
    correct = 0;

    std::string symbol;
    double a, b, c, alpha, beta, gamma;

    // read header row
    csv_reader.getRow(database);

    while (!database.eof()) {
        std::vector<std::string> row = csv_reader.getRow(database);

        if (row.size() < 8)
            continue;

        symbol = row[0];
        a = atof(row[2].c_str());
        b = atof(row[3].c_str());
        c = atof(row[4].c_str());
        alpha = atof(row[5].c_str()) * ohkl::deg;
        beta = atof(row[6].c_str()) * ohkl::deg;
        gamma = atof(row[7].c_str()) * ohkl::deg;

        std::string bravais;

        try {
            bravais = ohkl::SpaceGroup(symbol).bravaisTypeSymbol();
        } catch (...) {
            continue; // unknown space group
        }

        ++total;

        ohkl::UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        ohkl::UnitCell gruberCell(a, b, c, alpha, beta, gamma);
        ohkl::UnitCell cell;

        cell.setParameters(a, b, c, alpha, beta, gamma);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        ohkl::NiggliReduction niggli(niggliCell.metric(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);
        gruberCell.transform(niggli_P);

        // perform reduction using GruberReduction class
        Eigen::Matrix3d gruber_g, gruber_P;
        ohkl::LatticeCentring centering;
        ohkl::BravaisType bravaisType;
        ohkl::GruberReduction gruber(gruberCell.metric(), gruber_tolerance);

        try {
            gruber.reduce(gruber_P, centering, bravaisType);
        } catch (std::exception& e) {
            CHECK(false);
        }

        gruberCell.setBravaisType(bravaisType);
        gruberCell.setLatticeCentring(centering);

        if (gruberCell.bravaisTypeSymbol() == bravais) {
            ++correct;
            outfile << row[0] << '\t' << row[1] << '\t' << row[2] << '\t' << row[3] << '\t'
                    << row[4] << '\t' << row[5] << '\t' << row[6] << '\t' << row[7] << '\n';
        }

        CHECK(gruberCell.bravaisTypeSymbol()[0] == bravais[0]);
        CHECK(gruberCell.bravaisTypeSymbol()[1] == bravais[1]);
    }
}
