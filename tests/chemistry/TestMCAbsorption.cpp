#define BOOST_TEST_MODULE "Test Monte-Carlo Absorption"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/chemistry/Material.h>
#include <nsxlib/geometry/ConvexHull.h>
#include <nsxlib/geometry/MCAbsorption.h>
#include <nsxlib/utils/Types.h>
#include <nsxlib/utils/Units.h>

using namespace Eigen;

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_MCAbsorption)
{
    using CHullDouble = ConvexHull<double>;

    // Build an isotopically pure methane material
    sptrMaterial helium(new Material("He[3]"));

    // Create a cubic convex hull
    CHullDouble chull;
    chull.addVertex(Vector3d( 0, 0, 0));
    chull.addVertex(Vector3d( 1, 0, 0));
    chull.addVertex(Vector3d( 0, 1, 0));
    chull.addVertex(Vector3d( 0, 0, 1));
    chull.addVertex(Vector3d( 1, 1, 0));
    chull.addVertex(Vector3d( 1, 0, 1));
    chull.addVertex(Vector3d( 0, 1, 1));
    chull.addVertex(Vector3d( 1, 1, 1));
    chull.updateHull();
    chull.translateToCenter();
    chull.scale(0.032);

    // Create the MC absorption calculator
    MCAbsorption mca(3.2*cm,3.2*cm,-100);

    double muScattering=helium->muIncoherent();
    double muAbsorption=helium->muAbsorption(1.46e-10);

    // Set the material hull and its scattering and absorption attenuation factors
    mca.setSample(&chull,muScattering,muAbsorption);

    // Compute the transmission factor
    mca.run(10,Vector3d(0,1,0),Matrix3d::Identity());

    // Build an isotopically pure methane material
    sptrMaterial methane(new Material("CH4"));
    double mm=methane->molarMass();
    double volume=chull.getVolume();
    methane->setMassDensity(mm/volume);

    // Create the MC absorption calculator
    mca=nsx::MCAbsorption(3.2*cm,3.2*cm,-100);

    muScattering=methane->muIncoherent();
    muAbsorption=methane->muAbsorption(1.46e-10);

    // Set the material hull and its scattering and absorption attenuation factors
    mca.setSample(&chull,muScattering,muAbsorption);

    // Compute the transmission factor
    mca.run(10000,Vector3d(0,1,0),Matrix3d::Identity());
}
