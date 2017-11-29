#include <random>

#include <Eigen/Dense>

#include <nsxlib/Minimizer.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Profile3d.h>

const double eps = 1.0;

int main()
{
    const int n = 15;
    const int N = n*n*n;   
    Eigen::Matrix3d B;

    B <<
    2, 0, -1,
    1, 2, 0,
    0, -1, 3;

    const Eigen::Matrix3d reference_cov = B*B.transpose();
    Eigen::Matrix3d D = reference_cov.inverse();
    nsx::Profile3d reference(600, 1000, {50.0, 60.0, 70.0}, D);
    Eigen::ArrayXd x(N), y(N), z(N), I(N);

    int idx = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                x(idx) = 2*(i-n/2) + reference._c(0);
                y(idx) = 2*(j-n/2) + reference._c(1);
                z(idx) = 2*(k-n/2) + reference._c(2);
                ++idx;
            }
        }
    }

    // reference intensity profile
    I = reference.evaluate(x, y, z);
    const double reference_I = (I-reference._background).sum();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,60.0);

    // simulate noise to make fitting more difficult
    for (int i = 0; i < N; ++i) {
        I(i) += distribution(generator);
    }

    // easy test: run the fit with the original parameters
    nsx::Profile3d easyfit = reference.fit(x, y, z, I);

    NSX_CHECK_ASSERT(easyfit.success());

    const double easy_I = (easyfit.evaluate(x, y, z)-easyfit._background).sum();

    NSX_CHECK_CLOSE(reference_I, easy_I, 10);

    NSX_CHECK_CLOSE(easyfit._background, reference._background, 1);
    NSX_CHECK_CLOSE(easyfit._A, reference._A, 5);
    NSX_CHECK_CLOSE(easyfit._c(0), reference._c(0), 0.5);
    NSX_CHECK_CLOSE(easyfit._c(1), reference._c(1), 0.5);
    NSX_CHECK_CLOSE(easyfit._c(2), reference._c(2), 0.5);

    B << 
    easyfit._Dxx, easyfit._Dxy, easyfit._Dxz,
    easyfit._Dxy, easyfit._Dyy, easyfit._Dyz,
    easyfit._Dxz, easyfit._Dyz, easyfit._Dzz;

    const Eigen::Matrix3d easy_cov = B.inverse();

    NSX_CHECK_SMALL((reference_cov-easy_cov).norm() / reference_cov.norm(), 1e-1);


    // guess initial parameters from reference data
    nsx::Profile3d guess(x, y, z, I);
    
    D << guess._Dxx, guess._Dxy, guess._Dxz, guess._Dxy, guess._Dyy, guess._Dyz, guess._Dxz, guess._Dyz, guess._Dzz;

    // hard fit: using guessed parameters as input
    // check that it converges with few iterations
    nsx::Profile3d hardfit = guess.fit(x, y, z, I, 30);

    NSX_CHECK_ASSERT(hardfit.success());

    // check that the fit returns the same parameters
    NSX_CHECK_CLOSE(easyfit._background, hardfit._background, 1e-2);
    NSX_CHECK_CLOSE(easyfit._A, hardfit._A, 1e-2);
    NSX_CHECK_CLOSE(easyfit._c(0), hardfit._c(0), 1e-2);
    NSX_CHECK_CLOSE(easyfit._c(1), hardfit._c(1), 1e-2);
    NSX_CHECK_CLOSE(easyfit._c(2), hardfit._c(2), 1e-2);

    NSX_CHECK_CLOSE(easyfit._Dxx, hardfit._Dxx, 1e-2);
    NSX_CHECK_CLOSE(easyfit._Dxy, hardfit._Dxy, 1e-2);
    NSX_CHECK_CLOSE(easyfit._Dxz, hardfit._Dxz, 1e-2);
    NSX_CHECK_CLOSE(easyfit._Dyy, hardfit._Dyy, 1e-2);
    NSX_CHECK_CLOSE(easyfit._Dyz, hardfit._Dyz, 1e-2);
    NSX_CHECK_CLOSE(easyfit._Dzz, hardfit._Dzz, 1e-2);

    return 0;
}
