#define BOOST_TEST_MODULE "Test Sample"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/instrument/Axis.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/utils/Units.h>

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Sample)
{
    nsx::Sample s1("sample 1");
    nsx::sptrGonio g(new nsx::Gonio("Busing Levy convention"));
    g->addRotation("omega",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);
    g->addRotation("chi",Eigen::Vector3d(0,1,0),nsx::RotAxis::CCW);
    g->addRotation("phi",Eigen::Vector3d(0,0,1),nsx::RotAxis::CW);

    s1.setGonio(g);
    s1.setRestPosition(Eigen::Vector3d(0,0,0));
    nsx::ComponentState state(&s1, {90.0*nsx::deg,90.0*nsx::deg,0*nsx::deg});

    // Rotation should not affect the center of the sample since rest position is 0,0,0
    Eigen::Vector3d center=state.getPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_SMALL(center[1],tolerance);
    BOOST_CHECK_SMALL(center[2],tolerance);

    // Sample is off-centered, check that the same state change center
    s1.setRestPosition(Eigen::Vector3d(1.0,0.0,0.0));
    center=state.getPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_SMALL(center[1],tolerance);
    BOOST_CHECK_CLOSE(center[2],-1.0,tolerance);

    // Offseting one of the axis change center position
    auto g1=g->getAxis("chi");
    g1->setOffset(10.0*nsx::deg);
    center=state.getPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_CLOSE(center[1],sin(10*nsx::deg),tolerance);
    BOOST_CHECK_CLOSE(center[2],-cos(10*nsx::deg),tolerance);
}
