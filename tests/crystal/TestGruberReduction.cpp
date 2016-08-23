#define BOOST_TEST_MODULE "Test GruberReduction"
#define BOOST_TEST_DYN_LINK
#include "FlatDetector.h"
#include "Peak3D.h"
#include "Sample.h"
#include <boost/test/unit_test.hpp>
#include "Units.h"
#include <Eigen/Dense>
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

using namespace std;
using namespace SX::Crystal;
using namespace SX::Units;

const double tolerance=0.01;

int run_test()
{
    using vectord = vector<double>;

    ifstream database;
    database.open("crystal_database.csv", fstream::in);

    BOOST_CHECK(database.is_open());


    vector<char> buffer(1024, 0);
    vector<pair<string, vector<double>>> lattices;

    string symbol;
    double a, b, c, alpha, beta, gamma;

    while ( !database.eof()) {

        database.getline(&buffer[0], 1024);

        char* ptr = strtok(&buffer[0], ",");

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
            continue; // unknown space group
        }

        UnitCell cell(a, b, c, alpha, beta, gamma);
        NiggliReduction niggli(cell.getMetricTensor(), tolerance);
        Eigen::Matrix3d newg, P;
        niggli.reduce(newg, P);
        cell.transform(P);

        GruberReduction gruber(cell.getMetricTensor(), tolerance);

        LatticeCentring centering;
        BravaisType bravaisType;

        gruber.reduce(P, centering, bravaisType);
        cell.setBravaisType(bravaisType);
        cell.setLatticeCentring(centering);
        cell.transform(P);

        std::cout << "Bravais type:" << cell.getBravaisTypeSymbol()[0] << " expected " << bravais << std::endl;
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_GruberReduction)
{
    // todo: implement me!!
    BOOST_CHECK(run_test() == 0);
}
