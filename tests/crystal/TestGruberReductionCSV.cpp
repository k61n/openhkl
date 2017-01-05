#define BOOST_TEST_MODULE "Test GruberReductionCSV"
#define BOOST_TEST_DYN_LINK
#include "FlatDetector.h"
#include "Peak3D.h"
#include "Sample.h"
#include <boost/test/unit_test.hpp>
#include "Units.h"
#include <Eigen/Dense>
#include <Eigen/QR>
#include <vector>
#include "Gonio.h"
#include "Component.h"
#include "ComponentState.h"
#include "Source.h"
#include "Monochromator.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "UnitCell.h"
#include "SpaceGroup.h"

#include <memory>
#include <vector>
#include <utility>
#include <fstream>
#include <random>
#include <cmath>

#include "CSV.h"

using namespace std;
using namespace SX::Crystal;
using namespace SX::Units;
using namespace SX::Utils;

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;
// const double tolerance = 1e-4;

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
            //BOOST_FAIL("unknown space group");
            //std::cout << "unknown space group: " << symbol << std::endl;
            continue; // unknown space group
        }

        ++total;

        UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        UnitCell gruberCell(a, b, c, alpha, beta, gamma);
        UnitCell cell;

        cell.setParams(a, b, c, alpha, beta, gamma);

        // randomly transform the cell so that it is not in a normal form
        //Eigen::Matrix3d P = random_orthogonal_matrix();
        //niggliCell.transform(P);
        //gruberCell.transform(P);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        NiggliReduction niggli(niggliCell.getMetricTensor(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);
        gruberCell.transform(niggli_P);

        // testing
        // gruberCell.transform(niggli_P);

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
            std::cout << e.what() << std::endl;
        }

        ++counts[condition];


        gruberCell.setBravaisType(bravaisType);
        gruberCell.setLatticeCentring(centering);
        //gruberCell.transform(gruber_P);
        //gruber_g = gruberCell.getMetricTensor();

        // check agreement between niggli and gruber
//        BOOST_CHECK_CLOSE(niggliCell.getA(), gruberCell.getA(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getB(), gruberCell.getB(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getC(), gruberCell.getC(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getAlpha(), gruberCell.getAlpha(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getBeta(), gruberCell.getBeta(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getGamma(), gruberCell.getGamma(), tolerance);

        std::cout << symbol << " "
                  << a << " " << b << " " << c << " "
                  << alpha/deg << " " << beta/deg << " " << gamma/deg << " "
                  << bravais << " " << gruberCell.getBravaisTypeSymbol() << " "
                  << condition << " "
                  << correct*100.0/total << std::endl;

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

//        BOOST_CHECK_CLOSE(gruberCell.getA(), a, tolerance);
//        BOOST_CHECK_CLOSE(gruberCell.getB(), b, tolerance);
//        BOOST_CHECK_CLOSE(gruberCell.getC(), c, tolerance);
//        BOOST_CHECK_CLOSE(gruberCell.getAlpha(), alpha, tolerance);
//        BOOST_CHECK_CLOSE(gruberCell.getBeta(), beta, tolerance);
//        BOOST_CHECK_CLOSE(gruberCell.getGamma(), gamma, tolerance);
        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[0] == bravais[0]);
        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[1] == bravais[1]);
    }

    std::cout<< correct * 100.0 / total << "% correct out of " << total << " total" << std::endl;

    for (int i = 1; i < failures.size(); ++i)
        std::cout << i << " " << failures[i] << " " << counts[i] << std::endl;

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_GruberReductionCSV)
{
    BOOST_CHECK(run_test() == 0);
}
