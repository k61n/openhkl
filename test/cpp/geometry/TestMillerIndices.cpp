#include "test/cpp/catch.hpp"
#include <Eigen/Dense>

#include "core/crystal/MillerIndex.h"
TEST_CASE("test/geometry/TestMillerIndices.cpp", "") {

    Eigen::RowVector3i v1(1, 2, 3);

    nsx::MillerIndex hkl1(v1);

    int& x = hkl1[0];

    x = 100;

    CHECK(hkl1[0] == 100);
}
