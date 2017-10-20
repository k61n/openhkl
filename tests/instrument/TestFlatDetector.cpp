#include <cmath>

#include <Eigen/Dense>

#include <nsxlib/instrument/DetectorEvent.h>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/utils/NSXTest.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-3;

int main()
{
    #pragma message "todo: update this test"
    #if 0
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380*nsx::mm);
    d.setDimensions(80*nsx::mm,80*nsx::mm);
    d.setNPixels(32,32);

    // This should be the center of the detector at rest at (0,0.764,0)
    //nsx::DetectorEvent ev(&d, 15.5, 15.5, 0.0, {});
    Eigen::Vector3d center = d.getPos(15.5, 15.5);
    NSX_CHECK_SMALL(center[0],tolerance);
    NSX_CHECK_CLOSE(center[1],0.380,tolerance);
    NSX_CHECK_SMALL(center[2],tolerance);

    double gamma,nu;
    nsx::DetectorEvent ev2(&d, 15.5, 15.5, 0.0, {});
    ev2.getGammaNu(gamma, nu);
    NSX_CHECK_SMALL(gamma, tolerance);
    NSX_CHECK_SMALL(nu, tolerance);
    double th2 = ev2.get2Theta();
    NSX_CHECK_SMALL(th2, tolerance);

    // Attach a gonio
    nsx::sptrGonio g(new nsx::Gonio("gamma-arm"));
    g->addRotation("gamma",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    d.setGonio(g);

    nsx::DetectorEvent ev3(&d, 15.5, 15.5, 0.0, {90.0*nsx::deg});
    center=ev3.getPixelPosition();
    NSX_CHECK_CLOSE(center[0],0.380,tolerance);
    NSX_CHECK_SMALL(center[1],0.001);
    NSX_CHECK_SMALL(center[2],0.001);
    ev3.getGammaNu(gamma, nu);
    NSX_CHECK_CLOSE(gamma,90*nsx::deg,tolerance);
    NSX_CHECK_SMALL(nu,0.001);

    th2 = ev3.get2Theta();
    NSX_CHECK_CLOSE(th2,90.0*nsx::deg,tolerance);
    // Scattering in the center of the detector with wavelength 2.0
    // should get kf = (0.5,0,0)
    Eigen::Vector3d kf = ev3.getKf(2.0);
    NSX_CHECK_CLOSE(kf[0],0.5,tolerance);
    NSX_CHECK_SMALL(kf[1],0.001);
    NSX_CHECK_SMALL(kf[2],0.001);
    // Should be 45 deg in the x,-y plane
    Eigen::Vector3d Q = ev3.getQ(2.0);
    NSX_CHECK_CLOSE(Q[0],0.5,tolerance);
    NSX_CHECK_CLOSE(Q[1],-0.5,tolerance);
    NSX_CHECK_SMALL(Q[2],0.001);

    // Check that detector receive a scattering vector at the right position
    double px, py, t;
    Eigen::Vector3d from(0,0,0);
    d.receiveKf(px,py,Eigen::Vector3d(0,1,0),from,t,{0.0*nsx::deg});
    NSX_CHECK_CLOSE(px,15.5,tolerance);
    NSX_CHECK_CLOSE(py,15.5,tolerance);

    d.receiveKf(px,py,Eigen::Vector3d(1,0,0),from,t,{90.0*nsx::deg});
    NSX_CHECK_CLOSE(px,15.5,tolerance);
    NSX_CHECK_CLOSE(py,15.5,tolerance);

    // From is shifted up by length of 1 pixel along y and z
    from << 0,80/32.0*nsx::mm,80/32.0*nsx::mm;
    d.receiveKf(px,py,Eigen::Vector3d(1,0,0),from,t,{90.0*nsx::deg});
    NSX_CHECK_CLOSE(px,14.5,tolerance);
    NSX_CHECK_CLOSE(py,16.5,tolerance);

    return 0;
    #endif
}
