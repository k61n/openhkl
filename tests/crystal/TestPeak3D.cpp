#include <vector>

#include <Eigen/Dense>

#include <nsxlib/Component.h>
#include <nsxlib/ComponentState.h>
#include <nsxlib/FlatDetector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentTypes.h>
#include <nsxlib/Monochromator.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/Units.h>

const double tolerance=1e-6;

NSX_INIT_TEST

int main()
{
    #pragma message "TODO: implement this test"
    #if 0
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380*nsx::mm);
    d.setDimensions(80*nsx::mm,80*nsx::mm);
    d.setNPixels(32,32);

    //nsx::DetectorEvent event(&d, 15.5, 15.5, 0.0, {});

    nsx::sptrSource source(new nsx::Source);

    nsx::Monochromator mono("mono");

    source->addMonochromator(mono);
    source->setSelectedMonochromator(0);

    nsx::Peak3D peak;
    peak.setSource(source);
    peak.setDetectorEvent(event);

    Eigen::Vector3d Q=peak.getQ();
    NSX_CHECK_SMALL(Q[0],tolerance);
    NSX_CHECK_SMALL(Q[1],tolerance);
    NSX_CHECK_SMALL(Q[2],tolerance);

    nsx::sptrGonio g(new nsx::Gonio("Gamma"));
    g->addRotation("Gamma",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    d.setGonio(g);

    nsx::DetectorEvent event2(&d, 15.5,15.5, 0.0, {90.0*nsx::deg});
    peak.setDetectorEvent(event2);
    Q=peak.getQ();
    NSX_CHECK_CLOSE(Q[0],1.0,tolerance);
    NSX_CHECK_CLOSE(Q[1],-1.0,tolerance);
    NSX_CHECK_SMALL(Q[2],tolerance);

    nsx::Sample sample("sample");
    nsx::sptrGonio bl(new nsx::Gonio("Busing Levy convention"));
    bl->addRotation("omega",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    bl->addRotation("chi",Eigen::Vector3d(0,1,0),nsx::RotAxis::CCW);
    bl->addRotation("phi",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    sample.setGonio(bl);

    nsx::ComponentState state(&sample, {90.0*nsx::deg,0.0,0.0});

    peak.setSampleState(state);
    Q=peak.getQ();
    NSX_CHECK_CLOSE(Q[0],1.0,tolerance);
    NSX_CHECK_CLOSE(Q[1],1.0,tolerance);
    NSX_CHECK_SMALL(Q[2],tolerance);

    return 0;
    #endif
}
