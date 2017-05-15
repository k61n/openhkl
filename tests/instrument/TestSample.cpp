#define BOOST_TEST_MODULE "Test Sample"
#define BOOST_TEST_DYN_LINK
#include <nsxlib/instrument/Axis.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/instrument/Sample.h>
#include <Eigen/Geometry>
#include <boost/test/unit_test.hpp>
#include <nsxlib/instrument/Gonio.h>
#include <memory>

using namespace nsx::Units;
using namespace nsx::Instrument;
using Eigen::Vector3d;
const double tolerance=1e-6;


BOOST_AUTO_TEST_CASE(Test_Sample)
{
    Sample s1("sample 1");
    std::shared_ptr<Gonio> g(new Gonio("Busing Levy convention"));
    g->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
    g->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
    g->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);

    s1.setGonio(g);
    s1.setRestPosition(Eigen::Vector3d(0,0,0));
    ComponentState state(&s1, {90.0*deg,90.0*deg,0*deg});

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
    Axis* g1=g->getAxis("chi");
    g1->setOffsetFixed(false);
    g1->setOffset(10.0*deg);
    center=state.getPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_CLOSE(center[1],sin(10*deg),tolerance);
    BOOST_CHECK_CLOSE(center[2],-cos(10*deg),tolerance);

    // Set offset should have no effects if fixed
    g1->setOffsetFixed(true);
    g1->setOffset(30.0*deg);
    center=state.getPosition();
    BOOST_CHECK_SMALL(center[0],tolerance);
    BOOST_CHECK_CLOSE(center[1],sin(10*deg),tolerance);
    BOOST_CHECK_CLOSE(center[2],-cos(10*deg),tolerance);
}
