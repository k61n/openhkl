#include <cmath>

#include <Eigen/Dense>

#include "core/geometry/AABB.h"

const double tolerance = 1e-5;

TEST_CASE("test/geometry/TestAABB.cpp", "") {

    nsx::AABB bb;
    bb.setLower(Eigen::Vector3d(0, 0, 0));
    bb.setUpper(Eigen::Vector3d(1, 2, 3));
    Eigen::Vector3d center, extends;
    center = bb.center();
    extends = bb.extents();

    NSX_CHECK_CLOSE(center[0], 0.5, tolerance);
    NSX_CHECK_CLOSE(center[1], 1.0, tolerance);
    NSX_CHECK_CLOSE(center[2], 1.5, tolerance);

    NSX_CHECK_CLOSE(extends[0], 1, tolerance);
    NSX_CHECK_CLOSE(extends[1], 2, tolerance);
    NSX_CHECK_CLOSE(extends[2], 3, tolerance);
    // Check that a given point is inside
    NSX_CHECK_ASSERT(bb.isInside(Eigen::Vector3d(center)));
    // Check that a given point is outside
    NSX_CHECK_ASSERT(!bb.isInside(Eigen::Vector3d(2, 3, 4)));
    // A second bounding box, just touching
    nsx::AABB bb2(Eigen::Vector3d(1, 2, 3), Eigen::Vector3d(2, 3, 4));
    NSX_CHECK_ASSERT(bb2.collide(bb));
    // Second bounding box overlaps .
    bb2.setLower(Eigen::Vector3d(0.5, 2, 3));
    bb2.setUpper(Eigen::Vector3d(2, 3, 4));
    NSX_CHECK_ASSERT(bb2.collide(bb));
    // No overlap
    bb2.setLower(Eigen::Vector3d(2, 3, 4));
    bb2.setUpper(Eigen::Vector3d(4, 5, 6));
    NSX_CHECK_ASSERT(!(bb2.collide(bb)));

    // Translate and scale operations
    bb.translate(Eigen::Vector3d(1, 2, 3));
    Eigen::Vector3d lower = bb.lower();
    Eigen::Vector3d upper = bb.upper();
    NSX_CHECK_CLOSE(lower[0], 1.0, tolerance);
    NSX_CHECK_CLOSE(lower[1], 2.0, tolerance);
    NSX_CHECK_CLOSE(lower[2], 3.0, tolerance);
    NSX_CHECK_CLOSE(upper[0], 2.0, tolerance);
    NSX_CHECK_CLOSE(upper[1], 4.0, tolerance);
    NSX_CHECK_CLOSE(upper[2], 6.0, tolerance);
}
