#include <Eigen/Dense>

#include "core/crystal/MillerIndex.h"
#include "core/utils/NSXTest.h"

NSX_INIT_TEST

int main()
{

    Eigen::RowVector3i v1(1, 2, 3);

    nsx::MillerIndex hkl1(v1);

    int& x = hkl1[0];

    x = 100;

    NSX_CHECK_EQUAL(hkl1[0], 100);

    return 0;
}
