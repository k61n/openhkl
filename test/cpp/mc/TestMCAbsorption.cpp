#include "test/cpp/catch.hpp"

// TODO: contains no checks!

#include <Eigen/Dense>
#include <memory>

#include <xsection/Material.h>
#include "base/hull/ConvexHull.h"
#include "core/monte-carlo/MCAbsorption.h"
#include "base/utils/Units.h"

TEST_CASE("test/chemistry/TestMCAbsorption.cpp", "")
{

    // Build an isotopically pure methane material
    std::unique_ptr<xsection::Material> helium(new xsection::Material("He[3]"));

    // Create a cubic convex hull
    nsx::ConvexHull chull;
    chull.addVertex(Eigen::Vector3d(0, 0, 0));
    chull.addVertex(Eigen::Vector3d(1, 0, 0));
    chull.addVertex(Eigen::Vector3d(0, 1, 0));
    chull.addVertex(Eigen::Vector3d(0, 0, 1));
    chull.addVertex(Eigen::Vector3d(1, 1, 0));
    chull.addVertex(Eigen::Vector3d(1, 0, 1));
    chull.addVertex(Eigen::Vector3d(0, 1, 1));
    chull.addVertex(Eigen::Vector3d(1, 1, 1));
    chull.updateHull();
    chull.translateToCenter();
    chull.scale(0.032);

    double muScattering = helium->muIncoherent();
    double muAbsorption = helium->muAbsorption(1.46e-10);

    // Create the MC absorption calculator
    nsx::MCAbsorption mca(chull, 3.2 * nsx::cm, 3.2 * nsx::cm, -100);
    mca.setMuScattering(muScattering);
    mca.setMuAbsorption(muAbsorption);

    // Compute the transmission factor
    mca.run(10, Eigen::Vector3d(0, 1, 0), Eigen::Matrix3d::Identity());

    // Build an isotopically pure methane material
    std::unique_ptr<xsection::Material> methane(new xsection::Material("CH4"));
    double mm = methane->molarMass();
    double volume = chull.volume();
    methane->setMassDensity(mm / volume);

    // Create the MC absorption calculator
    mca = nsx::MCAbsorption(chull, 3.2 * nsx::cm, 3.2 * nsx::cm, -100);

    muScattering = methane->muIncoherent();
    muAbsorption = methane->muAbsorption(1.46e-10);

    // Set the material scattering and absorption attenuation factors
    mca.setMuScattering(muScattering);
    mca.setMuAbsorption(muAbsorption);

    // Compute the transmission factor
    mca.run(10000, Eigen::Vector3d(0, 1, 0), Eigen::Matrix3d::Identity());
}
