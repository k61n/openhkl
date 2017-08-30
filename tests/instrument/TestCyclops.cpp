#define BOOST_TEST_MODULE "Test Cyclops"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/DetectorEvent.h>
#include <nsxlib/instrument/Diffractometer.h>

BOOST_AUTO_TEST_CASE(Test_Cyclops)
{
    auto cyclop = nsx::Diffractometer::build("Cyclops");
    double g,nu;
    for (int i = 0; i < 7680; ++i) {
        for (int j = 0; j < 2400; ++j) {
            auto detector = cyclop->getDetector().get();
            nsx::DetectorEvent event(detector, i, j, 0.0,  {});
            event.getGammaNu(g, nu);
        }
    }
}
