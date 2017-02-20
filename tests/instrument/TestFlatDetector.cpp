#define BOOST_TEST_MODULE "Test Flat Detector"
#define BOOST_TEST_DYN_LINK
#include <nsxlib/utils/Units.h>
#include <cmath>
#include <Eigen/Dense>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <iostream>

using namespace SX::Units;
using namespace SX::Instrument;
using Eigen::Vector3d;
using Eigen::Matrix3d;
// 0.1% error
const double tolerance=1e-3;
BOOST_AUTO_TEST_CASE(Test_Flat_Detector)
{
    FlatDetector d("D10-detector");
    d.setDistance(380*mm);
    d.setDimensions(80*mm,80*mm);
    d.setNPixels(32,32);

    // This should be the center of the detector at rest at (0,0.764,0)
    Eigen::Vector3d center=d.getEventPosition(15.5,15.5);
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_CLOSE(center[1],0.380,tolerance);
    BOOST_CHECK_SMALL(center[2],tolerance);

    double gamma,nu;
    d.getGammaNu(15.5,15.5,gamma,nu);
    BOOST_CHECK_SMALL(gamma,tolerance);
    BOOST_CHECK_SMALL(nu,tolerance);
    double th2=d.get2Theta(15.5,15.5);
    BOOST_CHECK_SMALL(th2,tolerance);

    // Attach a gonio
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("gamma",Vector3d(0,0,1),RotAxis::CW);
    d.setGonio(g);

    center=d.getEventPosition(15.5,15.5,{90.0*deg});
    BOOST_CHECK_CLOSE(center[0],0.380,tolerance);
    BOOST_CHECK_SMALL(center[1],0.001);
    BOOST_CHECK_SMALL(center[2],0.001);
    d.getGammaNu(15.5,15.5,gamma,nu,{90.0*deg});
    BOOST_CHECK_CLOSE(gamma,90*deg,tolerance);
    BOOST_CHECK_SMALL(nu,0.001);

    th2=d.get2Theta(15.5,15.5,{90.0*deg});
    BOOST_CHECK_CLOSE(th2,90.0*deg,tolerance);
    // Scattering in the center of the detector with wavelength 2.0
    // should get kf = (0.5,0,0)
    Eigen::Vector3d kf=d.getKf(15.5,15.5,2.0,{90.0*deg});
    BOOST_CHECK_CLOSE(kf[0],0.5,tolerance);
    BOOST_CHECK_SMALL(kf[1],0.001);
    BOOST_CHECK_SMALL(kf[2],0.001);
    // Should be 45 deg in the x,-y plane
    Eigen::Vector3d Q=d.getQ(15.5,15.5,2.0,{90.0*deg});
    BOOST_CHECK_CLOSE(Q[0],0.5,tolerance);
    BOOST_CHECK_CLOSE(Q[1],-0.5,tolerance);
    BOOST_CHECK_SMALL(Q[2],0.001);

    // Check that detector receive a scattering vector at the right position
    double px, py, t;
    Eigen::Vector3d from(0,0,0);
    d.receiveKf(px,py,Vector3d(0,1,0),from,t,{0.0*deg});
    BOOST_CHECK_CLOSE(px,15.5,tolerance);
    BOOST_CHECK_CLOSE(py,15.5,tolerance);

    d.receiveKf(px,py,Vector3d(1,0,0),from,t,{90.0*deg});
    BOOST_CHECK_CLOSE(px,15.5,tolerance);
    BOOST_CHECK_CLOSE(py,15.5,tolerance);

    // From is shifted up by length of 1 pixel along y and z
    from << 0,80/32.0*mm,80/32.0*mm;
    d.receiveKf(px,py,Vector3d(1,0,0),from,t,{90.0*deg});
    BOOST_CHECK_CLOSE(px,14.5,tolerance);
    BOOST_CHECK_CLOSE(py,16.5,tolerance);


}

