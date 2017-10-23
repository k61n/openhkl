#include <cmath>
#include <map>
#include <random>
#include <utility>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/QR>

#include <nsxlib/Component.h>
#include <nsxlib/ComponentState.h>
#include <nsxlib/FlatDetector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/GruberReduction.h>
#include <nsxlib/NiggliReduction.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/Units.h>

const double gruber_tolerance = 1e-4;
const double niggli_tolerance = 1e-4;
const double tolerance = 1e-6;

int main()
{
    double A, B, C, D, E, F;

    A = 32.3232323232;
    B = 43.23232323;
    C = 35.35353535;
    D = 10.10101010;
    E = 5.15151515;
    F = 8.8888888;

    // scaling factor for a couple of special cases
    std::map<unsigned int, double> s;
    std::map<unsigned int, double> t;

    s[6]  = A / (2*D+F);
    s[7]  = A / (D+2*E);
    s[8]  = A / (D+E+F);
    s[16] = A / (2*D+F);
    s[17] = A / (D+E+F);
    s[24] = (B-A/3) / 2 / D;

    t[43] = (2*D+F) / B;
    s[43] = (2*(D+E+F)-t[43]*B) / A;


    std::vector<std::pair<std::string, std::vector<double>>> test_cases = {
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

        std::string expected_bravais(test_case.first);
        std::vector<double> p(test_case.second);

        for ( int i = 0; i < 10; ++i) {
            double A, B, C, D, E, F;

            A = p[0]+distribution(generator);
            B = p[1]+distribution(generator);
            C = p[2]+distribution(generator);
            D = p[3]+distribution(generator);
            E = p[4]+distribution(generator);
            F = p[5]+distribution(generator);

            nsx::UnitCell cell;
            cell.setABCDEF(A, B, C, D, E, F);

            Eigen::Matrix3d G = cell.metric();
            Eigen::Matrix3d P, NG, NP;
            NG = G;

            nsx::GruberReduction gruber(G, gruber_tolerance);
            nsx::BravaisType bravais_type;
            nsx::LatticeCentring centering;

            int match = gruber.reduce(P, centering, bravais_type);
            cell.setBravaisType(bravais_type);
            cell.setLatticeCentring(centering);

            cell.transform(P);

            NSX_CHECK_ASSERT(match == condition);
            NSX_CHECK_ASSERT(cell.getBravaisTypeSymbol() == expected_bravais);

            nsx::NiggliReduction niggli(G, niggli_tolerance);
            niggli.reduce(NG, NP);
        }
        ++condition;
    }
    return 0;
}
