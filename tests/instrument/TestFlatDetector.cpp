#define BOOST_TEST_MODULE "Test Flat Detector"
#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/instrument/DetectorEvent.h>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-3;

BOOST_AUTO_TEST_CASE(Test_Flat_Detector)
{
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380*nsx::mm);
    d.setDimensions(80*nsx::mm,80*nsx::mm);
    d.setNPixels(32,32);

    // This should be the center of the detector at rest at (0,0.764,0)
    nsx::DetectorEvent ev(d, 15.5, 15.5);
    Eigen::Vector3d center=ev.getPixelPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_CLOSE(center[1],0.380,tolerance);
    BOOST_CHECK_SMALL(center[2],tolerance);

    double gamma,nu;
    nsx::DetectorEvent ev2(d, 15.5, 15.5, {});
    ev2.getGammaNu(gamma, nu);
    BOOST_CHECK_SMALL(gamma, tolerance);
    BOOST_CHECK_SMALL(nu, tolerance);
    double th2 = ev2.get2Theta();
    BOOST_CHECK_SMALL(th2, tolerance);

    // Attach a gonio
    nsx::sptrGonio g(new nsx::Gonio("gamma-arm"));
    g->addRotation("gamma",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    d.setGonio(g);

    nsx::DetectorEvent ev3(d, 15.5, 15.5, {90.0*nsx::deg});
    center=ev3.getPixelPosition();
    BOOST_CHECK_CLOSE(center[0],0.380,tolerance);
    BOOST_CHECK_SMALL(center[1],0.001);
    BOOST_CHECK_SMALL(center[2],0.001);
    ev3.getGammaNu(gamma, nu);
    BOOST_CHECK_CLOSE(gamma,90*nsx::deg,tolerance);
    BOOST_CHECK_SMALL(nu,0.001);

    th2 = ev3.get2Theta();
    BOOST_CHECK_CLOSE(th2,90.0*nsx::deg,tolerance);
    // Scattering in the center of the detector with wavelength 2.0
    // should get kf = (0.5,0,0)
    Eigen::Vector3d kf = ev3.getKf(2.0);
    BOOST_CHECK_CLOSE(kf[0],0.5,tolerance);
    BOOST_CHECK_SMALL(kf[1],0.001);
    BOOST_CHECK_SMALL(kf[2],0.001);
    // Should be 45 deg in the x,-y plane
    Eigen::Vector3d Q = ev3.getQ(2.0);
    BOOST_CHECK_CLOSE(Q[0],0.5,tolerance);
    BOOST_CHECK_CLOSE(Q[1],-0.5,tolerance);
    BOOST_CHECK_SMALL(Q[2],0.001);

    // Check that detector receive a scattering vector at the right position
    double px, py, t;
    Eigen::Vector3d from(0,0,0);
    d.receiveKf(px,py,Eigen::Vector3d(0,1,0),from,t,{0.0*nsx::deg});
    BOOST_CHECK_CLOSE(px,15.5,tolerance);
    BOOST_CHECK_CLOSE(py,15.5,tolerance);

    d.receiveKf(px,py,Eigen::Vector3d(1,0,0),from,t,{90.0*nsx::deg});
    BOOST_CHECK_CLOSE(px,15.5,tolerance);
    BOOST_CHECK_CLOSE(py,15.5,tolerance);

    // From is shifted up by length of 1 pixel along y and z
    from << 0,80/32.0*nsx::mm,80/32.0*nsx::mm;
    d.receiveKf(px,py,Eigen::Vector3d(1,0,0),from,t,{90.0*nsx::deg});
    BOOST_CHECK_CLOSE(px,14.5,tolerance);
    BOOST_CHECK_CLOSE(py,16.5,tolerance);
}
