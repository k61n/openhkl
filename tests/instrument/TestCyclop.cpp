#define BOOST_TEST_MODULE "Test Cyclop"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/DetectorEvent.h>

BOOST_AUTO_TEST_CASE(Test_Cyclop)
{
    nsx::Instrument::DiffractometerStore* ds = nsx::Instrument::DiffractometerStore::Instance();
    nsx::Instrument::sptrDiffractometer cyclop = ds->buildDiffractomer("Cyclops");
    double g,nu;
    for (int i = 0; i < 7680; ++i) {
        for (int j = 0; j < 2400; ++j) {
            auto detector = cyclop->getDetector().get();
            nsx::Instrument::DetectorEvent event(*detector, i, j);
            event.getGammaNu(g, nu);
        }
    }
}
