#define BOOST_TEST_MODULE "Test GruberReductionCSV"
#define BOOST_TEST_DYN_LINK

#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/crystal/GruberReduction.h>
#include <nsxlib/crystal/NiggliReduction.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/instrument/Component.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/utils/CSV.h>
#include <nsxlib/utils/Units.h>

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;

int run_test()
{
    // using vectord = vector<double>;
    std::ofstream outfile;

    outfile.open("output.tsv", std::fstream::out);

    std::vector<int> failures(45, 0);
    std::vector<int> counts(45, 0);

    std::ifstream database;
    database.open("crystallography.tsv", std::fstream::in);
    nsx::CSV csv_reader('\t', '#');

    BOOST_CHECK(database.is_open());

    unsigned int total, correct;

    total = 0;
    correct = 0;

    std::string symbol;
    double a, b, c, alpha, beta, gamma;

    // read header row
    csv_reader.getRow(database);

    while ( !database.eof()) {

        std::vector<std::string> row =  csv_reader.getRow(database);

        if ( row.size() < 8)
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
            bravais = nsx::SpaceGroup(symbol).getBravaisTypeSymbol();
        }
        catch(...) {
            continue; // unknown space group
        }

        ++total;

        nsx::UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        nsx::UnitCell gruberCell(a, b, c, alpha, beta, gamma);
        nsx::UnitCell cell;

        cell.setParams(a, b, c, alpha, beta, gamma);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        nsx::NiggliReduction niggli(niggliCell.getMetricTensor(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);
        gruberCell.transform(niggli_P);

        // perform reduction using GruberReduction class
        Eigen::Matrix3d gruber_g, gruber_P;
        nsx::LatticeCentring centering;
        nsx::BravaisType bravaisType;
        nsx::GruberReduction gruber(gruberCell.getMetricTensor(), gruber_tolerance);

        int condition;

        try {
           condition = gruber.reduce(gruber_P, centering, bravaisType);
        }
        catch (std::exception& e) {
            BOOST_CHECK(false);
        }

        ++counts[condition];

        gruberCell.setBravaisType(bravaisType);
        gruberCell.setLatticeCentring(centering);

        if ( gruberCell.getBravaisTypeSymbol() == bravais) {
            ++correct;
            outfile << row[0] << '\t'
                    << row[1] << '\t'
                    << row[2] << '\t'
                    << row[3] << '\t'
                    << row[4] << '\t'
                    << row[5] << '\t'
                    << row[6] << '\t'
                    << row[7] << '\n';
        }
        else {
            ++failures[condition];
        }

        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[0] == bravais[0]);
        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[1] == bravais[1]);
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_GruberReductionCSV)
{
    BOOST_CHECK(run_test() == 0);
}
