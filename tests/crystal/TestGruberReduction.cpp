#define BOOST_TEST_MODULE "Test GruberReduction"
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

using namespace std;
using namespace SX::Crystal;
using namespace SX::Units;

const double niggli_tolerance = 1e-10;
const double gruber_tolerance = 1e-4;
const double tolerance = 1e-6;

Eigen::Matrix3d random_orthogonal_matrix()
{
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-1.0,1.0);


    Eigen::Matrix3d A;

    for (unsigned int i = 0; i < 3; ++i)
        for ( unsigned int j = 0; j < 3; ++j)
            A(i, j) = distribution(generator);

    Eigen::HouseholderQR<Eigen::Matrix3d> QR(A);

    return QR.householderQ();
}

int run_test()
{
    using vectord = vector<double>;

    ifstream database;
    database.open("crystal_database.csv", fstream::in);

    BOOST_CHECK(database.is_open());

    unsigned int total, correct;

    total = 0;
    correct = 0;


    vector<char> buffer(1024, 0);
    vector<pair<string, vector<double>>> lattices;

    string symbol;
    double a, b, c, alpha, beta, gamma;

    while ( !database.eof()) {

        database.getline(&buffer[0], 1024);

        char* ptr = strtok(&buffer[0], ",");

        // maybe we reached the end of the CSV file
        if ( ptr == nullptr && database.eof())
            continue;

        BOOST_CHECK(ptr != nullptr);
        symbol = string(ptr);

        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);
        a = atof(ptr);

        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);

        b = atof(ptr);
        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);
        c = atof(ptr);

        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);
        alpha = atof(ptr) * deg;

        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);
        beta = atof(ptr) * deg;

        ptr = strtok(nullptr, ",");
        BOOST_CHECK(ptr != nullptr);
        gamma = atof(ptr) * deg;

        char bravais;

        try {

            bravais = SpaceGroup(symbol).getBravaisType();
        }
        catch(...) {
            //BOOST_FAIL("unknown space group");
            std::cout << "unknown space group: " << symbol << std::endl;
            continue; // unknown space group
        }

        ++total;

        UnitCell niggliCell(a, b, c, alpha, beta, gamma);
        UnitCell gruberCell(a, b, c, alpha, beta, gamma);

        // randomly transform the cell so that it is not in a normal form
        Eigen::Matrix3d P = random_orthogonal_matrix();
        //niggliCell.transform(P);
        //gruberCell.transform(P);

        // perform reduction using NiggliReduction class
        Eigen::Matrix3d niggli_g, niggli_P;
        NiggliReduction niggli(niggliCell.getMetricTensor(), niggli_tolerance);
        niggli.reduce(niggli_g, niggli_P);
        niggliCell.transform(niggli_P);

        // testing
        // gruberCell.transform(niggli_P);

        // perform reduction using GruberReduction class
        Eigen::Matrix3d gruber_g, gruber_P;
        LatticeCentring centering;
        BravaisType bravaisType;
        GruberReduction gruber(gruberCell.getMetricTensor(), gruber_tolerance);
        gruber.reduce(gruber_P, centering, bravaisType);
        gruberCell.setBravaisType(bravaisType);
        gruberCell.setLatticeCentring(centering);
        gruberCell.transform(gruber_P);
        gruber_g = gruberCell.getMetricTensor();

        // check agreement between niggli and gruber
//        BOOST_CHECK_CLOSE(niggliCell.getA(), gruberCell.getA(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getB(), gruberCell.getB(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getC(), gruberCell.getC(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getAlpha(), gruberCell.getAlpha(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getBeta(), gruberCell.getBeta(), tolerance);
//        BOOST_CHECK_CLOSE(niggliCell.getGamma(), gruberCell.getGamma(), tolerance);

        BOOST_CHECK_CLOSE(gruberCell.getA(), a, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getB(), b, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getC(), c, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getAlpha(), alpha, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getBeta(), beta, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getGamma(), gamma, tolerance);


        std::cout << "Bravais type:" << gruberCell.getBravaisTypeSymbol()[0] << " expected " << bravais << std::endl;

        if ( gruberCell.getBravaisTypeSymbol()[0] == bravais)
            ++correct;
    }

    std::cout<< correct * 100.0 / total << "% correct out of " << total << " total" << std::endl;

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_GruberReduction)
{
    // todo: implement me!!
    BOOST_CHECK(run_test() == 0);
}
