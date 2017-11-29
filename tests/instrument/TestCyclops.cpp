#include <nsxlib/Detector.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/NSXTest.h>

int main()
{
    #pragma message "test not implemented"
    #if 0
    auto cyclop = nsx::Diffractometer::build("Cyclops");
    double g,nu;
    for (int i = 0; i < 7680; ++i) {
        for (int j = 0; j < 2400; ++j) {
            auto detector = cyclop->getDetector().get();
            nsx::DetectorEvent event(detector, i, j, 0.0,  {});
            event.getGammaNu(g, nu);
        }
    }

    return 0;
    #endif
}
