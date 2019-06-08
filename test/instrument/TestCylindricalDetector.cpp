#include <cmath>

#include <Eigen/Dense>

#include "core/detector/CylindricalDetector.h"
#include "core/axes/Gonio.h"
#include "core/instrument/InstrumentState.h"
#include "core/utils/Units.h"


const double tolerance = 1e-3;
TEST_CASE("test/instrument/TestCylindricalDetector.cpp", "") {


    nsx::CylindricalDetector d("D19-detector");
    d.setDistance(764 * nsx::mm);
    d.setAngularWidth(120.0 * nsx::deg);
    d.setHeight(40.0 * nsx::cm);
    d.setNCols(640);
    d.setNRows(256);

    nsx::DirectVector pixel_position = d.pixelPosition(319.5, 127.5);

    // This should be the center of the detector at rest at (0,0.764,0)
    Eigen::Vector3d center = pixel_position.vector();
    NSX_CHECK_SMALL(center[0], tolerance);
    NSX_CHECK_CLOSE(center[1], 0.764, tolerance);
    NSX_CHECK_SMALL(center[2], tolerance);

    // Create a fake instrument state
    nsx::InstrumentState state1(nullptr);

    // Should be center of the detector so gamma,nu=0 at rest
    double gamma = state1.gamma(pixel_position);
    NSX_CHECK_SMALL(gamma, tolerance);

    double nu = state1.nu(pixel_position);
    NSX_CHECK_SMALL(nu, tolerance);

    double th2 = state1.twoTheta(pixel_position);
    NSX_CHECK_SMALL(th2, tolerance);

    // Rotate the detector by 90 deg clockwise
    nsx::InstrumentState state2(nullptr);
    state2.detectorOrientation << 0, 1, 0, -1, 0, 0, 0, 0, 1;

    gamma = state2.gamma(pixel_position);
    NSX_CHECK_CLOSE(gamma, 90 * nsx::deg, tolerance);

    nu = state2.nu(pixel_position);
    NSX_CHECK_SMALL(nu, 0.001);

    th2 = state2.twoTheta(pixel_position);
    NSX_CHECK_CLOSE(th2, 90.0 * nsx::deg, tolerance);

    nsx::ReciprocalVector kf = state2.kfLab(pixel_position);

    NSX_CHECK_CLOSE(kf[0], 1, 0, tolerance);
    NSX_CHECK_SMALL(kf[1], tolerance);
    NSX_CHECK_SMALL(kf[2], tolerance);

    nsx::ReciprocalVector q = state2.sampleQ(pixel_position);

    NSX_CHECK_CLOSE(q[0], 1.0, tolerance);
    NSX_CHECK_CLOSE(q[1], -1.0, tolerance);
    NSX_CHECK_SMALL(q[2], tolerance);

    for (int i = d.minRow() + 3; i < d.maxRow() - 3; i += 2) {
        for (int j = d.minCol() + 3; j < d.maxCol() - 3; j += 2) {
            NSX_CHECK_EQ(d.hasPixel(j, i), true);

            auto position = d.pixelPosition(j, i);

            Eigen::Vector3d from(-1, -1, -1);
            from *= nsx::cm;
            Eigen::Vector3d kf = position.vector() - from;

            nsx::DetectorEvent event =
                d.constructEvent(nsx::DirectVector(from), nsx::ReciprocalVector(kf.transpose()));

            // detector has event
            NSX_CHECK_EQ(event._tof > 0.0, true);
            // time of flight is correct
            NSX_CHECK_CLOSE(event._tof, 1.0, 1e-5);
            // correct x coord
            NSX_CHECK_CLOSE(event._px, j, 1e-5);
            // correct x coord
            NSX_CHECK_CLOSE(event._py, i, 1e-5);
        }
    }

    return 0;
}
