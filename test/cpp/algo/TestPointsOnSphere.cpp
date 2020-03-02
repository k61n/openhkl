#include "test/cpp/catch.hpp"

#include "core/algo/FFTIndexing.h"
#include <iostream>

// For the time being, we check the distribution of points by visual inspection.
// So this test is not a real test; it just exports the points which we then
// visualize with whatever external tool.
TEST_CASE("test/algo/TestPointsOnSphere.cpp", "")
{
    int nVertices = 10000;
    std::vector<Eigen::RowVector3d> points = nsx::algo::pointsOnSphere(nVertices);
    for (const Eigen::RowVector3d point: points)
        std::cout << point[0] << " " << point[1] << " " << point[2] << "\n";
}
