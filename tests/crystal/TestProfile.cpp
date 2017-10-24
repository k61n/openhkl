#include <Eigen/Dense>

#include <nsxlib/NSXTest.h>
#include <nsxlib/Profile.h>

const double eps = 1.0;

#define fit_and_test(y) \
{ \
    nsx::Profile p; \
    NSX_CHECK_ASSERT(p.fit(y, 100) == true); \
    for (auto i = 0; i < y.size(); ++i) { \
        NSX_CHECK_CLOSE(y(i), p.evaluate(double(i)), eps); \
    } \
}

int main()
{
    const int size = 50;
    Eigen::VectorXd lorentz_test(size);
    Eigen::VectorXd gauss_test(size);
    Eigen::VectorXd voigt_test(size);
    Eigen::VectorXd noise_test(size);

    nsx::Lorentzian lor(4.0, 1.0, 14.0);
    nsx::Gaussian gauss(2.0, 14.0, 3.0);

    // simulated data
    for (auto i = 0; i < size; ++i) {
        const double x(i);
        lorentz_test(i) = 0.9*lor.evaluate(x)+0.1*gauss.evaluate(x);
        gauss_test(i) = 0.1*lor.evaluate(x)+0.9*gauss.evaluate(x);
        voigt_test(i) = 0.5*lorentz_test(i) + 0.5*gauss_test(i);
        noise_test(i) = (i%2) == 0 ? -1 : 1;
    }

    fit_and_test(lorentz_test);
    fit_and_test(gauss_test);
    fit_and_test(voigt_test);
    NSX_CHECK_ASSERT(nsx::Profile().fit(noise_test, 100) == false);

    return 0;
}
