#include <cmath>

#include <Eigen/Dense>

#include <core/DirectVector.h>
#include <core/FlatDetector.h>
#include <core/Gonio.h>
#include <core/InstrumentState.h>
#include <core/NSXTest.h>
#include <core/Units.h>

NSX_INIT_TEST

const double tolerance = 1e-3;

int main()
{
    nsx::FlatDetector d("D10-detector");
    d.setDistance(380 * nsx::mm);
    d.setWidth(80 * nsx::mm);
    d.setHeight(80 * nsx::mm);
    d.setNCols(32);
    d.setNRows(32);

    // This should be the center of the detector at rest at (0,0.764,0)
    nsx::DirectVector pixel_position = d.pixelPosition(15.5, 15.5);

    Eigen::Vector3d center = pixel_position.vector();
    NSX_CHECK_SMALL(center[0], tolerance);
    NSX_CHECK_CLOSE(center[1], 0.380, tolerance);
    NSX_CHECK_SMALL(center[2], tolerance);

    // Create a fake instrument state
    nsx::InstrumentState state1(nullptr);

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
    NSX_CHECK_SMALL(nu, tolerance);

    th2 = state2.twoTheta(pixel_position);
    NSX_CHECK_CLOSE(th2, 90 * nsx::deg, tolerance);

    nsx::ReciprocalVector kf = state2.kfLab(pixel_position);

    // Scattering in the center of the detector with wavelength 2.0
    NSX_CHECK_CLOSE(kf[0], 1, 0, tolerance);
    NSX_CHECK_SMALL(kf[1], tolerance);
    NSX_CHECK_SMALL(kf[2], tolerance);

    nsx::ReciprocalVector q = state2.sampleQ(pixel_position);

    // Should be 45 deg in the x,-y plane
    NSX_CHECK_CLOSE(q[0], 1.0, tolerance);
    NSX_CHECK_CLOSE(q[1], -1.0, tolerance);
    NSX_CHECK_SMALL(q[2], tolerance);

    for (int i = d.minRow() + 1; i < d.maxRow() - 1; i++) {

        for (int j = d.minCol() + 1; j < d.maxCol() - 1; j++) {

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
