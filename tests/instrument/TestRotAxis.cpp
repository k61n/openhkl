#include <Eigen/Dense>

#include <nsxlib/NSXTest.h>
#include <nsxlib/RotAxis.h>
#include <nsxlib/Units.h>

const double tolerance=1e-6;

int main()
{
    //Empty Rotation axis initialize to (0,0,1)
    nsx::RotAxis a("omega",Eigen::Vector3d(0,0,1));
    Eigen::Vector3d axis=a.getAxis();
    NSX_CHECK_ASSERT(axis[0]==0);
    NSX_CHECK_ASSERT(axis[1]==0);
    NSX_CHECK_ASSERT(axis[2]==1);
    //NSX_CHECK_SMALL(a.getOffset(),tolerance);
    a.setRotationDirection(nsx::RotAxis::CCW);
    // Check that a rotation CCW of 45.0 degrees brings the 1,0,0 vector into 1/2.(sqrt(2),sqrt(2),0)
    Eigen::Vector3d transf=a.transform(Eigen::Vector3d(1,0,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[2],tolerance);
    // Check same for CCW of 0,1,0
    transf=a.transform(Eigen::Vector3d(0,1,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],-0.5*sqrt(2.0),tolerance);
    NSX_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[2],tolerance);

    // Switch rotation direction CW
    a.setRotationDirection(nsx::RotAxis::CW);
    transf=a.transform(Eigen::Vector3d(1,0,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_CLOSE(transf[1],-0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[2],tolerance);
    // Check same for CW of 0,1,0
    transf=a.transform(Eigen::Vector3d(0,1,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[2],tolerance);

    // Ensure that offset is taken into account
    // todo: reenable this test after offset refactor
    #if 0
    a.setRotationDirection(nsx::RotAxis::CCW);
    a.setOffset(1.0*nsx::deg);
    transf=a.transform(Eigen::Vector3d(1,0,0),44.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_CLOSE(transf[1],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[2],tolerance);
    #endif

    // Switch rotation axis to y
    //a.setOffset(0.0);
    a.setAxis(Eigen::Vector3d(0,1,0));
    a.setRotationDirection(nsx::RotAxis::CCW);
    transf=a.transform(Eigen::Vector3d(1,0,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[1],tolerance);
    NSX_CHECK_CLOSE(transf[2],-0.5*sqrt(2.0),tolerance);

    // Test with Homogeneous matrix
    transf=a.transform(Eigen::Vector3d(1,0,0),45.0*nsx::deg);
    NSX_CHECK_CLOSE(transf[0],0.5*sqrt(2.0),tolerance);
    NSX_CHECK_SMALL(transf[1],tolerance);
    NSX_CHECK_CLOSE(transf[2],-0.5*sqrt(2.0),tolerance);

    return 0;
}
