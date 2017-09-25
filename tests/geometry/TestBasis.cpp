#include <Eigen/Dense>

#include <nsxlib/geometry/Basis.h>
#include <nsxlib/utils/NSXTest.h>

const double tolerance=1e-5;

int main()
{
    nsx::sptrBasis bprime(new nsx::Basis(Eigen::Vector3d(2,0,0),Eigen::Vector3d(0,2,0),Eigen::Vector3d(0,0,1)));

    nsx::Basis bsecond(Eigen::Vector3d(1,1,0),Eigen::Vector3d(-1,1,0),Eigen::Vector3d(0,0,1),bprime);

    Eigen::Vector3d x(1,0,0);

    Eigen::Vector3d xsecond=bsecond.fromStandard(x);

    //1
    NSX_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    NSX_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    NSX_CHECK_SMALL(xsecond(2),tolerance);

    //2
    x=bsecond.toStandard(xsecond);

    NSX_CHECK_CLOSE(x(0),1.0,tolerance);
    NSX_CHECK_SMALL(x(1),tolerance);
    NSX_CHECK_SMALL(x(2),tolerance);

    //3
    Eigen::RowVector3d xr(1,0,0);
    Eigen::RowVector3d xrsecond=bsecond.fromReciprocalStandard(xr);

    NSX_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    NSX_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    NSX_CHECK_SMALL(xrsecond(2),tolerance);

    //4
    xr=bsecond.toReciprocalStandard(xrsecond);

    NSX_CHECK_CLOSE(xr(0),1.0,tolerance);
    NSX_CHECK_SMALL(xr(1),tolerance);
    NSX_CHECK_SMALL(xr(2),tolerance);

    //5
    // Check the rebasing to the standard basis.
    bsecond.rebaseToStandard();

    xsecond=bsecond.fromStandard(x);

    NSX_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    NSX_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    NSX_CHECK_SMALL(xsecond(2),tolerance);

    //6
    x=bsecond.toStandard(xsecond);

    NSX_CHECK_CLOSE(x(0),1.0,tolerance);
    NSX_CHECK_SMALL(x(1),tolerance);
    NSX_CHECK_SMALL(x(2),tolerance);

    //7
    xrsecond=bsecond.fromReciprocalStandard(xr);

    NSX_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    NSX_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    NSX_CHECK_SMALL(xrsecond(2),tolerance);

    //8
    xr=bsecond.toReciprocalStandard(xrsecond);

    NSX_CHECK_CLOSE(xr(0),1.0,tolerance);
    NSX_CHECK_SMALL(xr(1),tolerance);
    NSX_CHECK_SMALL(xr(2),tolerance);

    //9
    // Check the rebasing to the first basis.
    bsecond.rebaseTo(bprime);

    xsecond=bsecond.fromStandard(x);

    NSX_CHECK_CLOSE(xsecond(0),0.25,tolerance);
    NSX_CHECK_CLOSE(xsecond(1),-0.25,tolerance);
    NSX_CHECK_SMALL(xsecond(2),tolerance);

    //10
    x=bsecond.toStandard(xsecond);

    NSX_CHECK_CLOSE(x(0),1.0,tolerance);
    NSX_CHECK_SMALL(x(1),tolerance);
    NSX_CHECK_SMALL(x(2),tolerance);

    //11
    xrsecond=bsecond.fromReciprocalStandard(xr);

    NSX_CHECK_CLOSE(xrsecond(0),2.0,tolerance);
    NSX_CHECK_CLOSE(xrsecond(1),-2.0,tolerance);
    NSX_CHECK_SMALL(xrsecond(2),tolerance);

    //12
    xr=bsecond.toReciprocalStandard(xrsecond);

    NSX_CHECK_CLOSE(xr(0),1.0,tolerance);
    NSX_CHECK_SMALL(xr(1),tolerance);
    NSX_CHECK_SMALL(xr(2),tolerance);

    nsx::sptrBasis reference(new nsx::Basis(nsx::Basis::fromDirectVectors(Eigen::Vector3d(1,0,0),
                                                                          Eigen::Vector3d(0,1,0),
                                                                          Eigen::Vector3d(0,0,1))));

    Eigen::Matrix3d P;

    // Check sigmas and error propagations
    NSX_CHECK_ASSERT(!reference->hasSigmas());

    // Set some errors along each direction of the basis vector.
    reference->setDirectSigmas(Eigen::Vector3d(0.010,0.000,0.000),
                               Eigen::Vector3d(0.000,0.010,0.000),
                               Eigen::Vector3d(0.000,0.000,0.010));

    NSX_CHECK_ASSERT(reference->hasSigmas());

    double err_a, err_b, err_c, err_alpha, err_beta, err_gamma;
    reference->getParametersSigmas(err_a,err_b,err_c,err_alpha,err_beta,err_gamma);

    NSX_CHECK_CLOSE(err_a,0.01,tolerance);
    NSX_CHECK_CLOSE(err_b,0.01,tolerance);
    NSX_CHECK_CLOSE(err_c,0.01,tolerance);
    NSX_CHECK_SMALL(err_alpha,tolerance);
    NSX_CHECK_SMALL(err_beta,tolerance);
    NSX_CHECK_SMALL(err_gamma,tolerance);
}
