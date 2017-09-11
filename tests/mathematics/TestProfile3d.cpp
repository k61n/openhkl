#define BOOST_TEST_MODULE "Test Profile 3d class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>
#include <random>
#include <iostream>

#include <nsxlib/mathematics/Profile3d.h>

const double eps = 1.0;

int run_test() 
{
    const int n = 15;
    const int N = n*n*n;
    nsx::ProfileParams p;
    p.background = 600;
    p.x0 = 50;
    p.y0 = 60;
    p.z0 = 70;
    p.A = 1000;
    
    Eigen::Matrix3d B;

    B <<
    2, 0, -1,
    1, 2, 0,
    0, -1, 3;

    const Eigen::Matrix3d reference_cov = B*B.transpose();
    Eigen::Matrix3d D = reference_cov.inverse();

    p.dxx = D(0,0);
    p.dxy = D(0,1);
    p.dxz = D(0,2);
    p.dyy = D(1,1);
    p.dyz = D(1,2);
    p.dzz = D(2,2);

    nsx::Profile3d reference(p);

    Eigen::ArrayXd x(N), y(N), z(N), I(N);

    int idx = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                x(idx) = 2*(i-n/2) + p.x0;
                y(idx) = 2*(j-n/2) + p.y0;
                z(idx) = 2*(k-n/2) + p.z0;
                ++idx;
            }
        }
    }

    // reference intensity profile
    I = reference.profile(x, y, z);
    const double reference_I = (I-p.background).sum();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0.0,60.0);

    // simulate noise to make fitting more difficult
    for (int i = 0; i < N; ++i) {
        I(i) += distribution(generator);
    }

    // easy test: run the fit with the original parameters
    nsx::Profile3d easyfit = reference.fit(x, y, z, I);

    BOOST_CHECK(easyfit.success() == true);

    nsx::ProfileParams easyp = easyfit.parameters();
    std::cout << "easy parameters \n" << easyp.pack().transpose() << std::endl;

    const double easy_I = (easyfit.profile(x, y, z)-easyp.background).sum();

    BOOST_CHECK_CLOSE(reference_I, easy_I, 10);

    BOOST_CHECK_CLOSE(easyp.background, p.background, 1);
    BOOST_CHECK_CLOSE(easyp.A, p.A, 5);
    BOOST_CHECK_CLOSE(easyp.x0, p.x0, 0.5);
    BOOST_CHECK_CLOSE(easyp.y0, p.y0, 0.5);
    BOOST_CHECK_CLOSE(easyp.z0, p.z0, 0.5);

    B << p.dxx, p.dxy, p.dxz, p.dxy, p.dyy, p.dyz, p.dxz, p.dyz, p.dzz;
    Eigen::Matrix3d easy_cov = B.inverse();

    BOOST_CHECK_SMALL((reference_cov-easy_cov).norm() / reference_cov.norm(), 1e-2);

    // guess initial parameters from reference data
    nsx::ProfileParams guessp(x, y, z, I);
    nsx::Profile3d guess(guessp);
    std::cout << "guessed parameters \n" << guess.parameters().pack().transpose() << std::endl;
    
    auto easypack = easyfit.parameters().pack();
    auto guesspack = guessp.pack();

    std::cout << "bkg = " << easypack(0) << "; " << guesspack(0) << "; " << (guesspack(0)-easypack(0))/easypack(0) << std::endl;
    std::cout << "x0 = " << easypack(1) << "; " << guesspack(1) << "; " << (guesspack(1)-easypack(1))/easypack(1) << std::endl;
    std::cout << "y0 = " << easypack(2) << "; " << guesspack(2) << "; " << (guesspack(2)-easypack(2))/easypack(2) << std::endl;
    std::cout << "z0 = " << easypack(3) << "; " << guesspack(3) << "; " << (guesspack(3)-easypack(3))/easypack(3) << std::endl;
    std::cout << "A = " << easypack(4) << "; " << guesspack(4) << "; " << (guesspack(4)-easypack(4))/easypack(4) << std::endl;
    
    D << guessp.dxx, guessp.dxy, guessp.dxz, guessp.dxy, guessp.dyy, guessp.dyz, guessp.dxz, guessp.dyz, guessp.dzz;
    const Eigen::Matrix3d guess_cov = D.inverse();

    std::cout << "dcov/cov = " << (guess_cov-easy_cov).norm() / easy_cov.norm();

    // hard fit: using guessed parameters as input
    // check that it converges with few iterations
    nsx::Profile3d hardfit = guess.fit(x, y, z, I, 15);

    BOOST_CHECK(hardfit.success() == true);
    nsx::ProfileParams hardp = hardfit.parameters();

    // check that the fit returns the same parameters
    BOOST_CHECK_CLOSE(easyp.background, hardp.background, 1e-2);
    BOOST_CHECK_CLOSE(easyp.A, hardp.A, 1e-2);
    BOOST_CHECK_CLOSE(easyp.x0, hardp.x0, 1e-2);
    BOOST_CHECK_CLOSE(easyp.y0, hardp.y0, 1e-2);
    BOOST_CHECK_CLOSE(easyp.z0, hardp.z0, 1e-2);

    BOOST_CHECK_CLOSE(easyp.dxx, hardp.dxx, 1e-2);
    BOOST_CHECK_CLOSE(easyp.dxy, hardp.dxy, 1e-2);
    BOOST_CHECK_CLOSE(easyp.dxz, hardp.dxz, 1e-2);
    BOOST_CHECK_CLOSE(easyp.dyy, hardp.dyy, 1e-2);
    BOOST_CHECK_CLOSE(easyp.dyz, hardp.dyz, 1e-2);
    BOOST_CHECK_CLOSE(easyp.dzz, hardp.dzz, 1e-2);

    for (int i = 0; i < 200; ++i) {
        nsx::Profile3d hardfit = guess.fit(x, y, z, I, 25);
    }

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Profile3d)
{
    BOOST_CHECK(run_test() == 0);
}
