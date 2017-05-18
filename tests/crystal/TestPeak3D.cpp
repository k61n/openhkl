#define BOOST_TEST_MODULE "Test Peak3D"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <memory>
#include <vector>

#include <Eigen/Dense>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/instrument/Component.h>
#include <nsxlib/instrument/ComponentState.h>
#include <nsxlib/instrument/FlatDetector.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Monochromator.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/Units.h>

using namespace nsx;

const double tolerance=1e-6;

BOOST_AUTO_TEST_CASE(Test_Peak3D)
{
    FlatDetector d("D10-detector");
    d.setDistance(380*mm);
    d.setDimensions(80*mm,80*mm);
    d.setNPixels(32,32);

    DetectorEvent event(d, 15.5, 15.5, {});

    std::shared_ptr<Source> source(new Source);

    nsx::Monochromator mono("mono");

    source->addMonochromator(mono);
    source->setSelectedMonochromator(0);

    Peak3D peak;
    peak.setSource(source);
    peak.setDetectorEvent(event);

    Eigen::Vector3d Q=peak.getQ();
    BOOST_CHECK_SMALL(Q[0],tolerance);
    BOOST_CHECK_SMALL(Q[1],tolerance);
    BOOST_CHECK_SMALL(Q[2],tolerance);

    std::shared_ptr<Gonio> g(new Gonio("Gamma"));
    g->addRotation("Gamma",Vector3d(0,0,1),RotAxis::CW);
    d.setGonio(g);

    DetectorEvent event2(d, 15.5,15.5,{90.0*deg});
    peak.setDetectorEvent(event2);
    Q=peak.getQ();
    BOOST_CHECK_CLOSE(Q[0],1,tolerance);
    BOOST_CHECK_CLOSE(Q[1],-1,tolerance);
    BOOST_CHECK_SMALL(Q[2],tolerance);

    Sample sample("sample");
    std::shared_ptr<Gonio> bl(new Gonio("Busing Levy convention"));
    bl->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
    bl->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
    bl->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);
    sample.setGonio(bl);

    ComponentState state(&sample, {90.0*deg,0.0,0.0});

    peak.setSampleState(state);
    Q=peak.getQ();
    BOOST_CHECK_CLOSE(Q[0],1,tolerance);
    BOOST_CHECK_CLOSE(Q[1],1,tolerance);
    BOOST_CHECK_SMALL(Q[2],tolerance);

    sample.setRestPosition(Eigen::Vector3d(0,0,0.01*m));
    Q=peak.getQ();
}
