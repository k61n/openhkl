#include <cmath>

#include <nsxlib/ErfInv.h>
#include <nsxlib/NSXTest.h>

int main()
{
    const double x_max = 5.0;
    const size_t count = 1000;

    for (size_t i = 0; i < count; ++i) {
        const double x = x_max * double(i) / double(count);
        const double y = std::erf(x);

        const double u = nsx::erf_inv(y);
        const double v = std::erf(u);

        NSX_CHECK_CLOSE(u, x, 1e-3);
        NSX_CHECK_CLOSE(v, y, 1e-13);
    }

    NSX_CHECK_CLOSE(nsx::getScale(0.382925), 0.5, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.682689), 1.0, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.866386), 1.5, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.954500), 2.0, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.987581), 2.5, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.997300), 3.0, 1e-2);
    NSX_CHECK_CLOSE(nsx::getScale(0.999535), 3.5, 1e-2);

    NSX_CHECK_CLOSE(nsx::getConfidence(0.5), 0.382925, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(1.0), 0.682689, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(1.5), 0.866386, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(2.0), 0.954500, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(2.5), 0.987581, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(3.0), 0.997300, 1e-2);
    NSX_CHECK_CLOSE(nsx::getConfidence(3.5), 0.999535, 1e-2);

    return 0;
}
