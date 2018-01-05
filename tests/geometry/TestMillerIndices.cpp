#include <Eigen/Dense>

#include <nsxlib/MillerIndices.h>
#include <nsxlib/NSXTest.h>

int main() {

    Eigen::RowVector3i v1(1,2,3);

    nsx::MillerIndices hkl1(v1);

    int& x = hkl1[0];

    x = 100;

    NSX_CHECK_EQUAL(hkl1[0],100);

    return 0;
}
