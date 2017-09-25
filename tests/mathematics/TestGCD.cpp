#include <nsxlib/mathematics/GCD.h>
#include <nsxlib/utils/NSXTest.h>

using namespace nsx;

int main()
{
    NSX_CHECK_ASSERT(nsx::gcd(3, 6) == 3);
    NSX_CHECK_ASSERT(nsx::gcd(3, 5) == 1);
    NSX_CHECK_ASSERT(nsx::gcd(10, 25, 15) == 5);
    NSX_CHECK_ASSERT(nsx::gcd(3, -3) == 3);
    NSX_CHECK_ASSERT(nsx::gcd(4, 12, 16) == 4);
    NSX_CHECK_ASSERT(nsx::gcd(12, 4, 16) == 4);
    NSX_CHECK_ASSERT(nsx::gcd(16, 12, 24) == 4);
    NSX_CHECK_ASSERT(nsx::gcd(16, 0) == 16);
    NSX_CHECK_ASSERT(nsx::gcd(0, 16) == 16);
}
