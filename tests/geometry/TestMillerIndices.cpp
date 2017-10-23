#include <Eigen/Dense>

#include <nsxlib/MillerIndices.h>
#include <nsxlib/NSXTest.h>

int main() {

    Eigen::RowVector3i v1(1,2,3);

    nsx::MillerIndices hkl1(v1);

    Eigen::RowVector3i v2 = v1 + static_cast<const Eigen::RowVector3i&>(hkl1);

    int& x = hkl1[0];

    x = 100;

    NSX_CHECK_EQUAL(hkl1[0],100);

    Eigen::RowVector3d hkld = static_cast<Eigen::RowVector3d>(hkl1);

    return 0;
}
