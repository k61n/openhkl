#include <cmath>

#include <Eigen/Dense>

#include <nsxlib/AABB.h>
#include <nsxlib/FitProfile.h>
#include <nsxlib/NSXTest.h>

NSX_INIT_TEST

int main()
{
    nsx::AABB bb;
    bb.setLower(Eigen::Vector3d(0,0,0));
    bb.setUpper(Eigen::Vector3d(1,2,3));

    nsx::FitProfile profile(bb, 10, 10, 10);
   
    NSX_CHECK_ASSERT(profile.addValue({0., 0., 0.}, 1.0) == true);
    NSX_CHECK_ASSERT(profile.addValue({1., 2., 3.}, 1.0) == false);

    NSX_CHECK_ASSERT(profile.addValue({3., 3., 3.}, 1.0) == false);
    NSX_CHECK_ASSERT(profile.addValue({0.5,1.0, 1.5}, 1.0) == true);

    NSX_CHECK_ASSERT(profile.addSubdividedValue({0.5,1.0, 1.5}, 1.0, 5) == 125);
    

    return 0;
}
