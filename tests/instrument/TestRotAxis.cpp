#include <Eigen/Dense>

#include "core/utils/NSXTest.h"
#include "core/axes/RotAxis.h"
#include "core/utils/Units.h"

const double tolerance = 1e-6;

NSX_INIT_TEST

int main()
{
    // Empty Rotation axis initialize to (0,0,1)
    nsx::RotAxis a("omega", Eigen::Vector3d(0, 0, 1));
    Eigen::Vector3d axis = a.axis();
    NSX_CHECK_ASSERT(axis[0] == 0);
    NSX_CHECK_ASSERT(axis[1] == 0);
    NSX_CHECK_ASSERT(axis[2] == 1);
    // NSX_CHECK_SMALL(a.getOffset(),tolerance);
    a.setRotationDirection(nsx::RotAxis::Direction::CCW);
    // Check that a rotation CCW of 45.0 degrees brings the 1,0,0 vector into
    // 1/2.(sqrt(2),sqrt(2),0)
    Eigen::Vector3d transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_CLOSE(transf[1], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[2], tolerance);
    // Check same for CCW of 0,1,0
    transf = a.transform(Eigen::Vector3d(0, 1, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], -0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_CLOSE(transf[1], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[2], tolerance);

    // Switch rotation direction CW
    a.setRotationDirection(nsx::RotAxis::Direction::CW);
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_CLOSE(transf[1], -0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[2], tolerance);
    // Check same for CW of 0,1,0
    transf = a.transform(Eigen::Vector3d(0, 1, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_CLOSE(transf[1], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[2], tolerance);

    // Switch rotation axis to y
    // a.setOffset(0.0);
    a.setAxis(Eigen::Vector3d(0, 1, 0));
    a.setRotationDirection(nsx::RotAxis::Direction::CCW);
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[1], tolerance);
    NSX_CHECK_CLOSE(transf[2], -0.5 * sqrt(2.0), tolerance);

    // Test with Homogeneous matrix
    transf = a.transform(Eigen::Vector3d(1, 0, 0), 45.0 * nsx::deg);
    NSX_CHECK_CLOSE(transf[0], 0.5 * sqrt(2.0), tolerance);
    NSX_CHECK_SMALL(transf[1], tolerance);
    NSX_CHECK_CLOSE(transf[2], -0.5 * sqrt(2.0), tolerance);

    return 0;
}
