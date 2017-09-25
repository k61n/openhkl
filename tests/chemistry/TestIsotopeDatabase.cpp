#include <nsxlib/chemistry/IsotopeDatabaseManager.h>
#include <nsxlib/utils/NSXTest.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-6;

int main()
{
    nsx::IsotopeDatabaseManager* imgr = nsx::IsotopeDatabaseManager::Instance();
    nsx::UnitsManager* um = nsx::UnitsManager::Instance();

    NSX_CHECK_EQUAL(imgr->getProperty<std::complex<double>>("Hf[176]","b_coherent"),std::complex<double>(6.61*1.0e-15,0));

    NSX_CHECK_ASSERT(imgr->hasProperty("Hf[176]","nuclear_spin"));

    NSX_CHECK_ASSERT(!imgr->hasProperty("Hf[176]","xxxxxx"));

    // Checks some of the property of the isotope
    NSX_CHECK_CLOSE(imgr->getProperty<double>("H[1]","molar_mass"),1.00782504*um->get("g_per_mole"),tolerance);
    NSX_CHECK_EQUAL(imgr->getProperty<int>("H[1]","n_protons"),1);
    NSX_CHECK_ASSERT(imgr->getProperty<bool>("H[1]","stable"));
    NSX_CHECK_ASSERT(imgr->getProperty<std::string>("H[1]","symbol").compare("H")==0);

    NSX_CHECK_EQUAL(imgr->getProperty<int>("Ag[107]","n_neutrons"),60);

    return 0;
}
