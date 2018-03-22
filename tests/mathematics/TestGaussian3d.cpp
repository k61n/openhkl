#include <random>

#include <Eigen/Dense>

#include <nsxlib/Minimizer.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Gaussian3d.h>

const double eps = 1.0;

int main()
{
    const int n = 30;
    const int N = n*n*n;   
    Eigen::Matrix3d B;

    B <<
    2, 0, -1,
    1, 2, 0,
    0, -1, 3;

    const Eigen::Matrix3d reference_cov = B*B.transpose();
    nsx::Gaussian3d reference(200, 1000, {50.0, 60.0, 70.0}, reference_cov);
    Eigen::ArrayXd x(N), y(N), z(N), I(N);

    int idx = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                x(idx) = 2*(i-n/2) + reference._center(0);
                y(idx) = 2*(j-n/2) + reference._center(1);
                z(idx) = 2*(k-n/2) + reference._center(2);
                ++idx;
            }
        }
    }

    // reference intensity profile
    I = reference.evaluate(x, y, z);
    const double reference_I = (I-reference._background).sum();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,1.0);

    // simulate Poisson noise to make fitting more difficult
    for (int i = 0; i < N; ++i) {
        I(i) += distribution(generator) * std::sqrt(I(i));
    }

    // easy test: run the fit with the original parameters
    nsx::Gaussian3d easy_fit = reference;
    easy_fit.fit(x, y, z, I);

    NSX_CHECK_ASSERT(easy_fit.success());

    const double easy_I = (easy_fit.evaluate(x, y, z)-easy_fit._background).sum();

    NSX_CHECK_CLOSE(reference_I, easy_I, 10);

    NSX_CHECK_CLOSE(easy_fit._background, reference._background, 1);
    NSX_CHECK_CLOSE(easy_fit._max, reference._max, 5);
    NSX_CHECK_CLOSE(easy_fit._center(0), reference._center(0), 0.5);
    NSX_CHECK_CLOSE(easy_fit._center(1), reference._center(1), 0.5);
    NSX_CHECK_CLOSE(easy_fit._center(2), reference._center(2), 0.5);

    B << 
    easy_fit._Dxx, easy_fit._Dxy, easy_fit._Dxz,
    easy_fit._Dxy, easy_fit._Dyy, easy_fit._Dyz,
    easy_fit._Dxz, easy_fit._Dyz, easy_fit._Dzz;

    const Eigen::Matrix3d easy_cov = B.inverse();

    NSX_CHECK_SMALL((reference_cov-easy_cov).norm() / reference_cov.norm(), 1e-1);

    // start with perturbed initial parameters
    nsx::Gaussian3d hard_fit = reference;

    hard_fit._background += 10;
    hard_fit._center += Eigen::Vector3d(2.0, -1.0, -1.0);
    hard_fit._max += 1.2;
    hard_fit._Dxx *= 1.1;
    hard_fit._Dxy *= 1.2;
    hard_fit._Dxz *= 0.9;
    hard_fit._Dyy *= 0.8;
    hard_fit._Dyz *= 1.1;
    hard_fit._Dzz *= 0.7;
  
    // hard fit: using guessed parameters as input
    // check that it converges with few iterations
    hard_fit.fit(x, y, z, I, 500);

    NSX_CHECK_ASSERT(hard_fit.success());

    // check that the fit returns the same parameters
    NSX_CHECK_CLOSE(easy_fit._background, hard_fit._background, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._max, hard_fit._max, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._center(0), hard_fit._center(0), 1e-2);
    NSX_CHECK_CLOSE(easy_fit._center(1), hard_fit._center(1), 1e-2);
    NSX_CHECK_CLOSE(easy_fit._center(2), hard_fit._center(2), 1e-2);

    NSX_CHECK_CLOSE(easy_fit._Dxx, hard_fit._Dxx, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._Dxy, hard_fit._Dxy, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._Dxz, hard_fit._Dxz, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._Dyy, hard_fit._Dyy, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._Dyz, hard_fit._Dyz, 1e-2);
    NSX_CHECK_CLOSE(easy_fit._Dzz, hard_fit._Dzz, 1e-2);

    std::cout << "easy_fit " << easy_fit._center.transpose() << std::endl;
    std::cout << easy_fit.covariance() << std::endl;

    std::cout << "hard_fit " << hard_fit._center.transpose() << std::endl;
    std::cout << hard_fit.covariance() << std::endl;

    double pearson = hard_fit.pearson(x, y, z, I);
    NSX_CHECK_GREATER_THAN(pearson, 0.79);
    std::cout << pearson << std::endl;

    return 0;
}
