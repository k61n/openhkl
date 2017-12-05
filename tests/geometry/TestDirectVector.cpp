#include <Eigen/Dense>

#include <nsxlib/DirectVector.h>
#include <nsxlib/NSXTest.h>

int main()
{
    Eigen::Vector3d v1(1,2,3);
    nsx::DirectVector dv1(v1);

    Eigen::Vector3d v2 = v1 + dv1.vector();
    double& x = dv1[0];
    x = 100;

    NSX_CHECK_EQUAL(dv1[0],100);

    return 0;
}
