#define BOOST_TEST_MODULE "Test Lattice Finder"
#define BOOST_TEST_DYN_LINK

#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/crystal/LatticeFinder.h>
#include <boost/test/unit_test.hpp>
#include <nsxlib/utils/Units.h>
#include <Eigen/Dense>
#include <vector>

using namespace SX::Crystal;
using namespace SX::Units;
// const double tolerance=1e-6;
BOOST_AUTO_TEST_CASE(Test_Lattice_Finder)
{
    UnitCell cell(5,5,9,90,90,90);
    SX::Crystal::LatticeFinder finder(1e-3,1e-3);
    for (int h=-3;h<4;++h) {
        for (int k=-3;k<4;++k) {
            for (int l=-3;l<4;++l) {
                    finder.addPoint(cell.toReciprocalStandard(Eigen::RowVector3d(h,k,l)));
                }
            }
    }
    finder.run(3.0);
    std::vector<SX::Crystal::LatticeVectors> solutions=finder.determineLattice(30,30);
}
