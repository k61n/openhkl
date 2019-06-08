#include "test/catch.hpp"
#include <Eigen/Dense>

#include "core/axes/TransAxis.h"

const double tolerance = 1e-6;

TEST_CASE("test/instrument/TestTransAxis.cpp", "") {

    nsx::TransAxis t("x", Eigen::Vector3d(1, 0, 0));

    // Translate the vector 0,1,2 by 0.4
    Eigen::Vector3d v = t.transform(Eigen::Vector3d(0, 1, 2), 0.4);

    CHECK(v[0] == Approx(0.4).epsilon(tolerance));
    CHECK(v[1] == Approx(1.0).epsilon(tolerance));
    CHECK(v[2] == Approx(2.0).epsilon(tolerance));
}
