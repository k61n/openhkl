#include "test/cpp/catch.hpp"
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "core/crystal/GruberReduction.h"
#include "core/crystal/NiggliReduction.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"
#include "core/gonio/Component.h"
#include "base/utils/CSV.h"
#include "base/utils/Units.h"

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;

TEST_CASE("test/crystal/TestGruberReductionCSV.cpp", "")
{

    // using vectord = vector<double>;
    std::ofstream outfile;

    outfile.open("output.tsv", std::fstream::out);

    std::ifstream database;
    database.open("crystallography.tsv", std::fstream::in);
    nsx::CSV csv_reader('\t', '#');

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
        alpha = atof(row[5].c_str()) * nsx::deg;
        beta = atof(row[6].c_str()) * nsx::deg;
        gamma = atof(row[7].c_str()) * nsx::deg;

        std::string bravais;

        try {
            bravais = nsx::SpaceGroup(symbol).bravaisTypeSymbol();
        } catch (...) {
            continue; // unknown space group
        }

        ++total;

        nsx::UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        nsx::UnitCell gruberCell(a, b, c, alpha, beta, gamma);
        nsx::UnitCell cell;

        cell.setParameters(a, b, c, alpha, beta, gamma);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        nsx::NiggliReduction niggli(niggliCell.metric(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);
        gruberCell.transform(niggli_P);

        // perform reduction using GruberReduction class
        Eigen::Matrix3d gruber_g, gruber_P;
        nsx::LatticeCentring centering;
        nsx::BravaisType bravaisType;
        nsx::GruberReduction gruber(gruberCell.metric(), gruber_tolerance);

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
