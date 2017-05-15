#define BOOST_TEST_MODULE "Test Monte-Carlo Absorption"
#define BOOST_TEST_DYN_LINK

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/ConvexHull.h>
#include <nsxlib/chemistry/Material.h>
#include <nsxlib/geometry/MCAbsorption.h>
#include <nsxlib/utils/Types.h>
#include <nsxlib/utils/Units.h>

// const double tolerance=1.0e-9;

using nsx::Chemistry::Material;
using nsx::Chemistry::sptrMaterial;
using nsx::Geometry::MCAbsorption;
using nsx::Units::cm;

BOOST_AUTO_TEST_CASE(Test_Material)
{
    typedef Eigen::Vector3d vector3;
    typedef nsx::Geometry::ConvexHull<double> CHullDouble;

    // Build an isotopically pure methane material
    sptrMaterial helium(new Material("He[3]"));

    // Create a cubic convex hull
    CHullDouble chull;
    chull.addVertex(vector3( 0, 0, 0));
    chull.addVertex(vector3( 1, 0, 0));
    chull.addVertex(vector3( 0, 1, 0));
    chull.addVertex(vector3( 0, 0, 1));
    chull.addVertex(vector3( 1, 1, 0));
    chull.addVertex(vector3( 1, 0, 1));
    chull.addVertex(vector3( 0, 1, 1));
    chull.addVertex(vector3( 1, 1, 1));
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
    mca.run(10,vector3(0,1,0),Eigen::Matrix3d::Identity());

    // Build an isotopically pure methane material
    sptrMaterial methane(new Material("CH4"));
    double mm=methane->molarMass();
    double volume=chull.getVolume();
    methane->setMassDensity(mm/volume);

    // Create the MC absorption calculator
    mca=nsx::Geometry::MCAbsorption(3.2*cm,3.2*cm,-100);

    muScattering=methane->muIncoherent();
    muAbsorption=methane->muAbsorption(1.46e-10);

    // Set the material hull and its scattering and absorption attenuation factors
    mca.setSample(&chull,muScattering,muAbsorption);

    // Compute the transmission factor
    mca.run(10000,vector3(0,1,0),Eigen::Matrix3d::Identity());
}
