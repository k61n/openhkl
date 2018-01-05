#include <Eigen/Dense>

#include <nsxlib/NSXTest.h>
#include <nsxlib/ReciprocalVector.h>

int main()
{

    Eigen::RowVector3d v1(1,2,3);

    nsx::ReciprocalVector rv1(v1);

    double& x = rv1[0];

    x = 100;

    NSX_CHECK_EQUAL(rv1[0],100);

    return 0;
}
