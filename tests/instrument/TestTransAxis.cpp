#include <Eigen/Dense>

#include <core/NSXTest.h>
#include <core/TransAxis.h>

const double tolerance=1e-6;

NSX_INIT_TEST

int main()
{
    nsx::TransAxis t("x",Eigen::Vector3d(1,0,0));

    // Translate the vector 0,1,2 by 0.4
    Eigen::Vector3d v=t.transform(Eigen::Vector3d(0,1,2),0.4);

    NSX_CHECK_CLOSE(v[0],0.4,tolerance);
    NSX_CHECK_CLOSE(v[1],1.0,tolerance);
    NSX_CHECK_CLOSE(v[2],2.0,tolerance);

    return 0;
}
