#include <cmath>

#include <Eigen/Dense>

#include <nsxlib/DirectVector.h>
#include <nsxlib/FlatDetector.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Units.h>

const double tolerance=1e-3;

int main()
{
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380*nsx::mm);
    d.setDimensions(80*nsx::mm,80*nsx::mm);
    d.setNPixels(32,32);

    // This should be the center of the detector at rest at (0,0.764,0)
    nsx::DirectVector pixel_position = d.pixelPosition(15.5, 15.5);

    Eigen::Vector3d center = pixel_position.vector();
    NSX_CHECK_SMALL(center[0],tolerance);
    NSX_CHECK_CLOSE(center[1],0.380,tolerance);
    NSX_CHECK_SMALL(center[2],tolerance);

    // Create a fake instrument state
    nsx::InstrumentState state1;
    state1.wavelength = 1.0;
    state1.ni = Eigen::Vector3d(0.0,1.0,0.0);
    state1.detectorOffset = Eigen::Vector3d::Zero();
    state1.detectorOrientation = Eigen::Matrix3d::Identity();
    state1.sampleOrientation = Eigen::Matrix3d::Identity();
    state1.samplePosition = Eigen::Vector3d::Zero();

    double gamma = state1.gamma(pixel_position);
    NSX_CHECK_SMALL(gamma, tolerance);

    double nu = state1.nu(pixel_position);
    NSX_CHECK_SMALL(nu, tolerance);

    double th2 = state1.twoTheta(pixel_position);
    NSX_CHECK_SMALL(th2, tolerance);


    // Rotate the detector by 90 deg clockwise
    nsx::InstrumentState state2;
    state2.wavelength = 1.0;
    state2.ni = Eigen::Vector3d(0.0,1.0,0.0);
    state2.detectorOffset = Eigen::Vector3d::Zero();
    state2.detectorOrientation <<  0, 1, 0,
                                  -1, 0, 0,
                                   0, 0, 1;
    state2.sampleOrientation = Eigen::Matrix3d::Identity();
    state2.samplePosition = Eigen::Vector3d::Zero();

    gamma = state2.gamma(pixel_position);
    NSX_CHECK_CLOSE(gamma, 90*nsx::deg, tolerance);

    nu = state2.nu(pixel_position);
    NSX_CHECK_SMALL(nu, tolerance);

    th2 = state2.twoTheta(pixel_position);
    NSX_CHECK_CLOSE(th2, 90*nsx::deg, tolerance);

    nsx::ReciprocalVector kf = state2.kfLab(pixel_position);

    // Scattering in the center of the detector with wavelength 2.0
    NSX_CHECK_CLOSE(kf[0],1,0,tolerance);
    NSX_CHECK_SMALL(kf[1],tolerance);
    NSX_CHECK_SMALL(kf[2],tolerance);

    nsx::ReciprocalVector q = state2.sampleQ(pixel_position);

    // Should be 45 deg in the x,-y plane
    NSX_CHECK_CLOSE(q[0], 1.0,tolerance);
    NSX_CHECK_CLOSE(q[1],-1.0,tolerance);
    NSX_CHECK_SMALL(q[2], tolerance);

    return 0;
}
