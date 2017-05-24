#define BOOST_TEST_MODULE "Test Basis"
#define BOOST_TEST_DYN_LINK

#include <memory>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/Basis.h>

using Eigen::Vector3d;

using namespace nsx;

const double tolerance=1e-5;

BOOST_AUTO_TEST_CASE(Test_Basis)
{
    std::shared_ptr<Basis> bprime(new Basis(Vector3d(2,0,0),Vector3d(0,2,0),Vector3d(0,0,1)));

    Basis bsecond(Vector3d(1,1,0),Vector3d(-1,1,0),Vector3d(0,0,1),bprime);

    Vector3d x(1,0,0);

    Vector3d xsecond=bsecond.fromStandard(x);

    BOOST_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    BOOST_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    BOOST_CHECK_SMALL(xsecond(2),tolerance);

    x=bsecond.toStandard(xsecond);

    BOOST_CHECK_CLOSE(x(0),1.0,tolerance);
    BOOST_CHECK_SMALL(x(1),tolerance);
    BOOST_CHECK_SMALL(x(2),tolerance);

    Eigen::RowVector3d xr(1,0,0);
    Eigen::RowVector3d xrsecond=bsecond.fromReciprocalStandard(xr);

    BOOST_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    BOOST_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    BOOST_CHECK_SMALL(xrsecond(2),tolerance);

    xr=bsecond.toReciprocalStandard(xrsecond);

    BOOST_CHECK_CLOSE(xr(0),1.0,tolerance);
    BOOST_CHECK_SMALL(xr(1),tolerance);
    BOOST_CHECK_SMALL(xr(2),tolerance);

    // Check the rebasing to the standard basis.
    bsecond.rebaseToStandard();

    xsecond=bsecond.fromStandard(x);

    BOOST_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    BOOST_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    BOOST_CHECK_SMALL(xsecond(2),tolerance);

    x=bsecond.toStandard(xsecond);

    BOOST_CHECK_CLOSE(x(0),1.0,tolerance);
    BOOST_CHECK_SMALL(x(1),tolerance);
    BOOST_CHECK_SMALL(x(2),tolerance);

    xrsecond=bsecond.fromReciprocalStandard(xr);

    BOOST_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    BOOST_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    BOOST_CHECK_SMALL(xrsecond(2),tolerance);

    xr=bsecond.toReciprocalStandard(xrsecond);

    BOOST_CHECK_CLOSE(xr(0),1.0,tolerance);
    BOOST_CHECK_SMALL(xr(1),tolerance);
    BOOST_CHECK_SMALL(xr(2),tolerance);

    // Check the rebasing to the first basis.
    bsecond.rebaseTo(bprime);

    xsecond=bsecond.fromStandard(x);

    BOOST_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    BOOST_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    BOOST_CHECK_SMALL(xsecond(2),tolerance);

    x=bsecond.toStandard(xsecond);

    BOOST_CHECK_CLOSE(x(0),1.0,tolerance);
    BOOST_CHECK_SMALL(x(1),tolerance);
    BOOST_CHECK_SMALL(x(2),tolerance);

    xrsecond=bsecond.fromReciprocalStandard(xr);

    BOOST_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    BOOST_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    BOOST_CHECK_SMALL(xrsecond(2),tolerance);

    xr=bsecond.toReciprocalStandard(xrsecond);

    BOOST_CHECK_CLOSE(xr(0),1.0,tolerance);
    BOOST_CHECK_SMALL(xr(1),tolerance);
    BOOST_CHECK_SMALL(xr(2),tolerance);

    std::shared_ptr<Basis> reference(new Basis(Basis::fromDirectVectors(Vector3d(1,0,0),Vector3d(0,1,0),Vector3d(0,0,1))));

    Eigen::Matrix3d P;

    // Check sigmas and error propagations
    BOOST_CHECK(!reference->hasSigmas());

    // Set some errors along each direction of the basis vector.
    reference->setDirectSigmas(Vector3d(0.010,0.000,0.000),Vector3d(0.000,0.010,0.000),Vector3d(0.000,0.000,0.010));

    BOOST_CHECK(reference->hasSigmas());

    double err_a, err_b, err_c, err_alpha, err_beta, err_gamma;
    reference->getParametersSigmas(err_a,err_b,err_c,err_alpha,err_beta,err_gamma);

    BOOST_CHECK_CLOSE(err_a,0.01,tolerance);
    BOOST_CHECK_CLOSE(err_b,0.01,tolerance);
    BOOST_CHECK_CLOSE(err_c,0.01,tolerance);
    BOOST_CHECK_SMALL(err_alpha,tolerance);
    BOOST_CHECK_SMALL(err_beta,tolerance);
    BOOST_CHECK_SMALL(err_gamma,tolerance);

    P << 0,2,0,4,0,0,0,0,-1;
    reference->transform(P);

    reference->getParametersSigmas(err_a,err_b,err_c,err_alpha,err_beta,err_gamma);

    double err_as, err_bs, err_cs, err_alphas, err_betas, err_gammas;
    reference->getReciprocalParametersSigmas(err_as,err_bs,err_cs,err_alphas,err_betas,err_gammas);
}
