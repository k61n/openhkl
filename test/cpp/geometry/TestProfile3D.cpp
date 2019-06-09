#include "test/cpp/catch.hpp"
#include <cmath>

#include <Eigen/Dense>

#include "core/geometry/AABB.h"
#include "core/integration/Profile3D.h"

TEST_CASE("test/geometry/TestProfile3D.cpp", "")
{

    nsx::AABB bb;
    bb.setLower(Eigen::Vector3d(0, 0, 0));
    bb.setUpper(Eigen::Vector3d(1, 2, 3));

    nsx::Profile3D profile(bb, 10, 10, 10);

    CHECK(profile.addValue({0., 0., 0.}, 1.0) == true);
    CHECK(profile.addValue({1., 2., 3.}, 1.0) == false);

    CHECK(profile.addValue({3., 3., 3.}, 1.0) == false);
    CHECK(profile.addValue({0.5, 1.0, 1.5}, 1.0) == true);

    CHECK(profile.addSubdividedValue({0.5, 1.0, 1.5}, 1.0, 5) == 125);
}
