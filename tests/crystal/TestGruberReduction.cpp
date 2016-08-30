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
#include <cmath>

#include "RandomMatrix.h"

using namespace std;
using namespace SX::Crystal;
using namespace SX::Units;
using namespace SX::Utils;

const double niggli_tolerance = 1e-9;
const double gruber_tolerance = 1e-5;
const double tolerance = 1e-4;

int run_test()
{
    double a, b, c, alpha, beta, gamma;
    string bravais;

    a = b = c = 11.301;
    alpha = beta = gamma = 52.93*deg;
    bravais = "hR";

    UnitCell niggliCell(a, b, c, alpha, beta, gamma);
    UnitCell gruberCell(a, b, c, alpha, beta, gamma);

    for (unsigned int i = 0; i < 5; ++i) {
        std::cout << "iteration " << i << std::endl;

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

        int condition;

        try {
            condition = gruber.reduce(gruber_P, centering, bravaisType);
        }
        catch (std::exception& e) {
            BOOST_CHECK(false);
            std::cout << e.what() << std::endl;
        }

        gruberCell.setBravaisType(bravaisType);
        gruberCell.setLatticeCentring(centering);
        gruberCell.transform(gruber_P);
        gruber_g = gruberCell.getMetricTensor();

        // check agreement between niggli and gruber
                BOOST_CHECK_CLOSE(niggliCell.getA(), gruberCell.getA(), tolerance);
                BOOST_CHECK_CLOSE(niggliCell.getB(), gruberCell.getB(), tolerance);
                BOOST_CHECK_CLOSE(niggliCell.getC(), gruberCell.getC(), tolerance);
                BOOST_CHECK_CLOSE(niggliCell.getAlpha(), gruberCell.getAlpha(), tolerance);
                BOOST_CHECK_CLOSE(niggliCell.getBeta(), gruberCell.getBeta(), tolerance);
                BOOST_CHECK_CLOSE(niggliCell.getGamma(), gruberCell.getGamma(), tolerance);

//        std::cout <<  " "
//                   << a << " " << b << " " << c << " "
//                   << alpha/deg << " " << beta/deg << " " << gamma/deg << " "
//                   << bravais << " " << gruberCell.getBravaisTypeSymbol() << " "
//                   << condition << " "
//                   << correct*100.0/total << std::endl;

//        if ( gruberCell.getBravaisTypeSymbol() == bravais)
//            ++correct;
//        else {
//            ++failures[condition];
//        }

        BOOST_CHECK_CLOSE(gruberCell.getA(), a, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getB(), b, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getC(), c, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getAlpha(), alpha, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getBeta(), beta, tolerance);
        BOOST_CHECK_CLOSE(gruberCell.getGamma(), gamma, tolerance);
        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[0] == bravais[0]);
        BOOST_CHECK(gruberCell.getBravaisTypeSymbol()[1] == bravais[1]);
    }

    return 0;
}



BOOST_AUTO_TEST_CASE(Test_GruberReduction)
{
    // todo: implement me!!
    BOOST_CHECK(run_test() == 0);
}
