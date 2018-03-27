#include <Eigen/Dense>

#include <nsxlib/ChemistryTypes.h>
#include <nsxlib/ConvexHull.h>
#include <nsxlib/Material.h>
#include <nsxlib/MCAbsorption.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

NSX_INIT_TEST

int main()
{
    // Build an isotopically pure methane material
    nsx::sptrMaterial helium(new nsx::Material("He[3]"));

    // Create a cubic convex hull
    nsx::ConvexHull chull;
    chull.addVertex(Eigen::Vector3d( 0, 0, 0));
    chull.addVertex(Eigen::Vector3d( 1, 0, 0));
    chull.addVertex(Eigen::Vector3d( 0, 1, 0));
    chull.addVertex(Eigen::Vector3d( 0, 0, 1));
    chull.addVertex(Eigen::Vector3d( 1, 1, 0));
    chull.addVertex(Eigen::Vector3d( 1, 0, 1));
    chull.addVertex(Eigen::Vector3d( 0, 1, 1));
    chull.addVertex(Eigen::Vector3d( 1, 1, 1));
    chull.updateHull();
    chull.translateToCenter();
    chull.scale(0.032);

    // Create the MC absorption calculator
    nsx::MCAbsorption mca(3.2*nsx::cm,3.2*nsx::cm,-100);

    double muScattering=helium->muIncoherent();
    double muAbsorption=helium->muAbsorption(1.46e-10);

    // Set the material hull and its scattering and absorption attenuation factors
    mca.setSample(&chull,muScattering,muAbsorption);

    // Compute the transmission factor
    mca.run(10,Eigen::Vector3d(0,1,0),Eigen::Matrix3d::Identity());

    // Build an isotopically pure methane material
    nsx::sptrMaterial methane(new nsx::Material("CH4"));
    double mm=methane->molarMass();
    double volume=chull.getVolume();
    methane->setMassDensity(mm/volume);

    // Create the MC absorption calculator
    mca=nsx::MCAbsorption(3.2*nsx::cm,3.2*nsx::cm,-100);

    muScattering=methane->muIncoherent();
    muAbsorption=methane->muAbsorption(1.46e-10);

    // Set the material hull and its scattering and absorption attenuation factors
    mca.setSample(&chull,muScattering,muAbsorption);

    // Compute the transmission factor
    mca.run(10000,Eigen::Vector3d(0,1,0),Eigen::Matrix3d::Identity());

    return 0;
}
