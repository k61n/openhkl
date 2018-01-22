#include <Eigen/Dense>

#include <nsxlib/Ellipsoid.h>
#include <nsxlib/IntegrationRegion.h>
#include <nsxlib/NSXTest.h>

int main()
{
    const Eigen::Vector3d center(5,5,5);
    const Eigen::Vector3d radii(3,3,3);
    Eigen::Matrix3d axes;
    axes << 1, 0, 0,
            0, 1, 0,
            0, 0, 1;

    nsx::Ellipsoid ell(center,radii,axes);

    nsx::IntegrationRegion region(ell,1.0,2.0,4);
    region.setBestSlice(4);

    // Test that those points are neither in any integration slice neither in background
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d( 12,  54, 25)),-1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d( 42, -5 ,  8)),-1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d( 19,  0,   1)),-1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d( -7,  2,   6)),-1);

    // Test that those points are in background
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(1,5,5)),0);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5,9,5)),0);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5,5,0)),0);

    // Test that those points are in one integration slice
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5.0,5,5)),1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5.0,5,5.1)),1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5.0,5.2,5)),1);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5.0,5.9,5)),2);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(6.2,5,5)),2);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(6.5,5,5)),3);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(7.0,5,5)),3);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(5.2,5,7.4)),4);
    NSX_CHECK_EQUAL(region.classifySlice(Eigen::Vector3d(7.7,5,5)),4);

    Eigen::MatrixXi mask(11,11);
    mask.setConstant(-1);

    region.updateMask(mask,5);

    NSX_CHECK_EQUAL(mask(0,0),-1);
    NSX_CHECK_EQUAL(mask(1,0),-1);
    NSX_CHECK_EQUAL(mask(2,0), 0);
    NSX_CHECK_EQUAL(mask(3,0), 0);
    NSX_CHECK_EQUAL(mask(4,0), 0);
    NSX_CHECK_EQUAL(mask(5,0), 0);
    NSX_CHECK_EQUAL(mask(1,1), 0);
    NSX_CHECK_EQUAL(mask(2,2), 0);
    NSX_CHECK_EQUAL(mask(3,3), 4);
    NSX_CHECK_EQUAL(mask(4,3), 3);
    NSX_CHECK_EQUAL(mask(4,4), 2);
    NSX_CHECK_EQUAL(mask(5,5), 1);

}
