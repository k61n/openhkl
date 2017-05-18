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

using namespace std;
using namespace nsx;

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;

int run_test()
{
    // using vectord = vector<double>;
    ofstream outfile;

    outfile.open("output.tsv", fstream::out);

    vector<int> failures(45, 0);
    vector<int> counts(45, 0);

    ifstream database;
    database.open("crystallography.tsv", fstream::in);
    CSV csv_reader('\t', '#');

    BOOST_CHECK(database.is_open());

    unsigned int total, correct;

    total = 0;
    correct = 0;

    string symbol;
    double a, b, c, alpha, beta, gamma;

    // read header row
    csv_reader.getRow(database);

    while ( !database.eof()) {

        vector<string> row =  csv_reader.getRow(database);

        if ( row.size() < 8)
            continue;

        symbol = row[0];
        a = atof(row[2].c_str());
        b = atof(row[3].c_str());
        c = atof(row[4].c_str());
        alpha = atof(row[5].c_str()) * deg;
        beta = atof(row[6].c_str()) * deg;
        gamma = atof(row[7].c_str()) * deg;

        std::string bravais;

        try {
            bravais = SpaceGroup(symbol).getBravaisTypeSymbol();
        }
        catch(...) {
            continue; // unknown space group
        }

        ++total;

        UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        UnitCell gruberCell(a, b, c, alpha, beta, gamma);
        UnitCell cell;

        cell.setParams(a, b, c, alpha, beta, gamma);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        NiggliReduction niggli(niggliCell.getMetricTensor(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);
        gruberCell.transform(niggli_P);

        // perform reduction using GruberReduction class
        Eigen::Matrix3d gruber_g, gruber_P;
        LatticeCentring centering;
        BravaisType bravaisType;
        GruberReduction gruber(gruberCell.getMetricTensor(), gruber_tolerance);

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
