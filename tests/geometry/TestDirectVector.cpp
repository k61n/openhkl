#include <Eigen/Dense>

#include <nsxlib/geometry/DirectVector.h>
#include <nsxlib/utils/NSXTest.h>

int main()
{

    Eigen::Vector3d v1(1,2,3);

    nsx::DirectVector dv1(v1);

    Eigen::Vector3d v2 = v1 + static_cast<const Eigen::Vector3d&>(dv1);

    double& x = dv1[0];

    x = 100;

    NSX_CHECK_EQUAL(dv1[0],100);

    return 0;
}
