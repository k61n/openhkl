#include <Eigen/Dense>

#include <nsxlib/geometry/ReciprocalVector.h>
#include <nsxlib/utils/NSXTest.h>

int main()
{

    Eigen::RowVector3d v1(1,2,3);

    nsx::ReciprocalVector rv1(v1);

    Eigen::RowVector3d v2 = v1 + static_cast<const Eigen::RowVector3d&>(rv1);

    double& x = rv1[0];

    x = 100;

    NSX_CHECK_EQUAL(rv1[0],100);

    return 0;
}
