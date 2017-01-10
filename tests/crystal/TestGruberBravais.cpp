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
#include <map>

using namespace std;
using namespace SX::Crystal;
using namespace SX::Units;

const double gruber_tolerance = 1e-4;
const double niggli_tolerance = 1e-4;
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

UnitCell cell_from_params(double A, double B, double C, double D, double E, double F)
{
    double a = sqrt(A);
    double b = sqrt(B);
    double c = sqrt(C);

    double alpha = acos(D / b / c);
    double beta = acos(E / a / c);
    double gamma = acos(F / a / b);

    UnitCell cell(a, b, c, alpha, beta, gamma);
    Eigen::Matrix3d g = cell.getMetricTensor();

    BOOST_CHECK_CLOSE(g(0,0), A, tolerance);
    BOOST_CHECK_CLOSE(g(1,1), B, tolerance);
    BOOST_CHECK_CLOSE(g(2,2), C, tolerance);
    BOOST_CHECK_CLOSE(g(1,2), D, tolerance);
    BOOST_CHECK_CLOSE(g(0,2), E, tolerance);
    BOOST_CHECK_CLOSE(g(0,1), F, tolerance);

    return cell;
}

int run_test()
{
    double A, B, C, D, E, F;

    A = 32.3232323232;
    B = 43.23232323;
    C = 35.35353535;
    D = 10.10101010;
    E = 5.15151515;
    F = 8.8888888;

    // scaling factor for a couple of special cases
    map<unsigned int, double> s;
    map<unsigned int, double> t;

    s[6]  = A / (2*D+F);
    s[7]  = A / (D+2*E);
    s[8]  = A / (D+E+F);
    s[16] = A / (2*D+F);
    s[17] = A / (D+E+F);
    s[24] = (B-A/3) / 2 / D;

    t[43] = (2*D+F) / B;
    s[43] = (2*(D+E+F)-t[43]*B) / A;


    vector<pair<string, vector<double>>> test_cases = {
        {"cF", {A, A, A, A/2, A/2, A/2}},  // condition 1
        {"hR", {A, A, A, D, D, D}},   // condition 2
        {"cP", {A, A, A, 0, 0, 0}},   // condition 3
        {"hR", {A, A, A, -D, -D, -D}},   // condition 4
        {"cI", {A, A, A, -A/3, -A/3, -A/3}},   // condition 5
        {"tI", {A, A, A, -D*s[6], -D*s[6], -F*s[6]}},   // condition 6
        {"tI", {A, A, A, -D*s[7], -E*s[7], -E*s[7]}},   // condition 7
        {"oI", {A, A, A, -D*s[8], -E*s[8], -F*s[8]}},   // condition 8
        {"hR", {A, A, C, A/2, A/2, A/2}},   // condition 9
        {"mC", {A, A, C, D, D, F}},   // condition 10
        {"tP", {A, A, C, 0, 0, 0}},   // condition 11
        {"hP", {A, A, C, 0, 0, -A/2}},   // condition 12
        {"oC", {A, A, C, 0, 0, F}},   // condition 13
        {"mC", {A, A, C, -D, -D, -F}},   // condition 14
        {"tI", {A, A, C, -A/2, -A/2, 0}},   // condition 15
        {"oF", {A, A, C, -D*s[16], -D*s[16], -F*s[16]}},   // condition 16
        {"mC", {A, A, C, -D*s[17], -E*s[17], -F*s[17]}},   // condition 17
        {"tI", {A, B, B, A/4, A/2, A/2}},   // condition 18
        {"oI", {A, B, B, D, A/2, A/2}},   // condition 19
        {"mC", {A, B, B, D, E, E}},   // condition 20
        {"tP", {A, B, B, 0, 0, 0}},   // condition 21
        {"hP", {A, B, B, -B/2, 0, 0}},   // condition 22
        {"oC", {A, B, B, D, 0, 0}},   // condition 23
        {"hR", {A, B, B, -D*s[24], -A/3, -A/3}},   // condition 24
        {"mC", {A, B, B, -D, E, E}},   // condition 25
        {"oF", {A, B, C, A/4, A/2, A/2}},   // condition 26
        {"mC", {A, B, C, D, A/2, A/2}},   // condition 27
        {"mC", {A, B, C, D, A/2, 2*D}},   // condition 28
        {"mC", {A, B, C, D, 2*D, A/2}},   // condition 29
        {"mC", {A, B, C, B/2, E, 2*E}},   // condition 30
        {"aP", {A, B, C, D, E, F}},   // condition 31
        {"oP", {A, B, C, 0, 0, 0}},   // condition 32
        {"mP", {A, B, C, 0, E, 0}},   // condition 33
        {"mP", {A, B, C, 0, 0, F}},   // condition 34
        {"mP", {A, B, C, D, 0, 0}},   // condition 35
        {"oC", {A, B, C, 0, -A/2, 0}},   // condition 36
        {"mC", {A, B, C, D, -A/2, 0}},   // condition 37
        {"oC", {A, B, C, 0, 0, -A/2}},   // condition 38
        {"mC", {A, B, C, D, 0, -A/2}},   // condition 39
        {"oC", {A, B, C, -B/2, 0, 0}},   // condition 40
        {"mC", {A, B, C, -B/2, E, 0}},   // condition 41
        {"oI", {A, B, C, -B/2, -A/2, 0}},   // condition 42
        {"mI", {A*s[43], B*t[43], C, -D, -E, -F}},   // condition 43
        {"aP", {A, B, C, -D, -E, -F}},   // condition 44
    };

    int condition = 1;

    for (auto&& test_case: test_cases) {
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(-0.1*gruber_tolerance, 0.1*gruber_tolerance);

        string expected_bravais(test_case.first);
        vector<double> p(test_case.second);

        for ( int i = 0; i < 10; ++i) {
            double a, b, c, alpha, beta, gamma;
            double A, B, C, D, E, F;

            A = p[0]+distribution(generator);
            B = p[1]+distribution(generator);
            C = p[2]+distribution(generator);
            D = p[3]+distribution(generator);
            E = p[4]+distribution(generator);
            F = p[5]+distribution(generator);

            a = std::sqrt(A);
            b = std::sqrt(B);
            c = std::sqrt(C);
            alpha = std::acos(D / b / c);
            beta = std::acos(E / a / c);
            gamma = std::acos(F / a / b);

            UnitCell cell(a, b, c, alpha, beta, gamma);
            Eigen::Matrix3d G = cell.getMetricTensor();

            // G = 0.5 * (G + G.transpose());
            // double det = G.determinant();

            Eigen::Matrix3d P, NG, NP;

            NG = G;

            GruberReduction gruber(G, gruber_tolerance);
            BravaisType bravais_type;
            LatticeCentring centering;

            int match = gruber.reduce(P, centering, bravais_type);
            cell.setBravaisType(bravais_type);
            cell.setLatticeCentring(centering);

            cell.transform(P);

            cout << "test case " << condition << " match: " << match << endl;

            if ( condition != match) {
                cout << "error" << endl;
            }

            BOOST_CHECK(match == condition);
            BOOST_CHECK(cell.getBravaisTypeSymbol() == expected_bravais);

            NiggliReduction niggli(G, niggli_tolerance);
            niggli.reduce(NG, NP);
        }
        ++condition;
    }


    return 0;
}

BOOST_AUTO_TEST_CASE(Test_GruberBravais)
{
    BOOST_CHECK(run_test() == 0);
}
