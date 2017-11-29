#include <nsxlib/Gaussian.h>
#include <nsxlib/NSXTest.h>

const double eps = 1e-10;

int main()
{
    nsx::Gaussian g1(2.0, 1.0, 3.0);

    NSX_CHECK_CLOSE(g1.evaluate(1.0), 4.0, eps);

    return 0;
}
