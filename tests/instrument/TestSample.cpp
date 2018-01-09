#include <Eigen/Dense>

#include <nsxlib/Axis.h>
#include <nsxlib/ComponentState.h>
#include <nsxlib/DirectVector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Units.h>

const double tolerance=1e-6;

int main()
{
    nsx::Sample s1("sample 1");
    nsx::sptrGonio g(new nsx::Gonio("Busing Levy convention"));
    g->addRotation("omega",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    g->addRotation("chi",Eigen::Vector3d(0,1,0),nsx::RotAxis::CCW);
    g->addRotation("phi",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);

    s1.setGonio(g);
    s1.setRestPosition(nsx::DirectVector(Eigen::Vector3d(0,0,0)));
    nsx::ComponentState state(&s1, {90.0*nsx::deg,90.0*nsx::deg,0*nsx::deg});

    // Rotation should not affect the center of the sample since rest position is 0,0,0
    nsx::DirectVector center = state.getPosition();
    NSX_CHECK_SMALL(center[0],tolerance);
    NSX_CHECK_SMALL(center[1],tolerance);
    NSX_CHECK_SMALL(center[2],tolerance);

    // Sample is off-centered, check that the same state change center
    s1.setRestPosition(nsx::DirectVector(Eigen::Vector3d(1.0,0.0,0.0)));
    center=state.getPosition();
    NSX_CHECK_SMALL(center[0],tolerance);
    NSX_CHECK_SMALL(center[1],tolerance);
    NSX_CHECK_CLOSE(center[2],-1.0,tolerance);

    // Offseting one of the axis change center position
    // todo: reenable this test after offset refactor
    #if 0
    auto g1=g->getAxis("chi");
    g1->setOffset(10.0*nsx::deg);
    center=state.getPosition();
    NSX_CHECK_SMALL(center[0],tolerance);
    NSX_CHECK_CLOSE(center[1],sin(10*nsx::deg),tolerance);
    NSX_CHECK_CLOSE(center[2],-cos(10*nsx::deg),tolerance);
    #endif

    return 0;
}
