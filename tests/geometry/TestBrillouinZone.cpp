#include <cmath>

#include <Eigen/Dense>

#include <core/BrillouinZone.h>
#include <core/NSXTest.h>
#include <core/UnitCell.h>

const double tolerance=1e-9;

// note: validation data obtained from web, see e.g.
// http://lampx.tugraz.at/~hadley/ss1/bzones/
// https://en.wikipedia.org/wiki/Brillouin_zone


NSX_INIT_TEST

// validation function
void validate_zone(const Eigen::Matrix3d& B, int nverts, int nfaces)
{
    nsx::BrillouinZone zone(B, 1e-3);
    nsx::ConvexHull hull = zone.convexHull();
    
    NSX_CHECK_EQUAL(hull.nVertices(), nverts);
    NSX_CHECK_EQUAL(zone.vertices().size(), nverts);
    NSX_CHECK_EQUAL(2*zone.normals().size(), nfaces);

    for (auto n: zone.normals()) {
        NSX_CHECK_ASSERT(zone.inside(0.5*n));
        NSX_CHECK_ASSERT(zone.inside(-0.5*n));

        NSX_CHECK_ASSERT(hull.contains(0.49*n) == true);
        NSX_CHECK_ASSERT(hull.contains(0.51*n) == false);

        NSX_CHECK_ASSERT(zone.inside(0.51*n) == false);
        NSX_CHECK_ASSERT(zone.inside(-0.51*n) == false);
    }

    for (auto v: zone.vertices()) {
        NSX_CHECK_ASSERT(zone.inside(v));
        NSX_CHECK_ASSERT(zone.inside(-v));

        NSX_CHECK_ASSERT(zone.inside(1.01*v) == false);
        NSX_CHECK_ASSERT(zone.inside(-1.01*v) == false);
    }    

    NSX_CHECK_CLOSE(hull.volume(), std::fabs(B.determinant()), 1e-8);
}

int main()
{
    const double deg = M_PI / 180.0;
    nsx::UnitCell uc;

    // todo: write this test!!
    Eigen::Matrix3d B;

    // primitive triclinic aP
    uc.setParameters(20, 25, 15, 95*deg, 85*deg, 93*deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // face-centered cubic cF
    uc.setParameters(10, 10, 10, 30*deg, 30*deg, 30*deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive cubic cP
    uc.setParameters(10, 10, 10, 90*deg, 90*deg, 90*deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // primitive monoclinic mP
    uc.setParameters(15, 20, 25, 78*deg, 90*deg, 90*deg);
    validate_zone(uc.reciprocalBasis(), 12, 8);

    // face-centered monoclinic mC
    uc.setParameters(20, 20, 25, 78*deg, 78*deg, 85*deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive orthorhombic oP
    uc.setParameters(15, 20, 25, 90*deg, 90*deg, 90*deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // face-centered orthorhombic oC

    // all faces centered orthorhombic oF

    // body-centered orthorhombic oI
    const double oI_beta = std::acos(0.5 * std::sqrt(15.0 / 20.0));
    uc.setParameters(15, 20, 20, 78*deg, oI_beta, oI_beta);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // primitive tetragonal tP
    uc.setParameters(20, 25, 25, 90*deg, 90*deg, 90*deg);
    validate_zone(uc.reciprocalBasis(), 8, 6);

    // body-centered tetragonal tI
    const double tI_alpha = std::acos(0.5*20.0/25.0);
    uc.setParameters(20, 20, 25, -tI_alpha, -tI_alpha, 90*deg);
    validate_zone(uc.reciprocalBasis(), 24, 14);

    // hP 
    // hR

    // cP
    // cF
    // cI

    return 0;
}

