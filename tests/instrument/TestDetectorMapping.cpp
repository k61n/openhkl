#define BOOST_TEST_MODULE "Test Detector Mapping"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "DetectorMapping.h"
#include "DetectorMappingFactory.h"

using namespace SX::Instrument;

BOOST_AUTO_TEST_CASE(Test_Detector_Mapping)
{

    unsigned int newpx,newpy;

    const unsigned int px=20;
    const unsigned int py=300;

    const unsigned int nrows=100;
    const unsigned int ncols=500;

    IDetectorMapping* map;

    DetectorMappingFactory *f=DetectorMappingFactory::Instance();

    f->registerCallback("bottomleft-ccw",&BottomLeftMappingCCW::create);
    map=f->create("bottomleft-ccw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,px);
    BOOST_CHECK_EQUAL(newpy,py);

    f->registerCallback("bottomleft-cw",&BottomLeftMappingCW::create);
    map=f->create("bottomleft-cw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,py);
    BOOST_CHECK_EQUAL(newpy,px);

    f->registerCallback("bottomright-ccw",&BottomRightMappingCCW::create);
    map=f->create("bottomright-ccw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,ncols-py);
    BOOST_CHECK_EQUAL(newpy,px);

    f->registerCallback("bottomright-cw",&BottomRightMappingCW::create);
    map=f->create("bottomright-cw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,ncols-px);
    BOOST_CHECK_EQUAL(newpy,py);

    f->registerCallback("topleft-ccw",&TopLeftMappingCCW::create);
    map=f->create("topleft-ccw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,py);
    BOOST_CHECK_EQUAL(newpy,nrows-px);

    f->registerCallback("topleft-cw",&TopLeftMappingCW::create);
    map=f->create("topleft-cw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,px);
    BOOST_CHECK_EQUAL(newpy,nrows-py);

    f->registerCallback("topright-ccw",&TopRightMappingCCW::create);
    map=f->create("topright-ccw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,ncols-px);
    BOOST_CHECK_EQUAL(newpy,nrows-py);

    f->registerCallback("topright-cw",&TopRightMappingCW::create);
    map=f->create("topright-cw",nrows,ncols);
    (*map)(px,py,newpx,newpy);
    BOOST_CHECK_EQUAL(newpx,ncols-py);
    BOOST_CHECK_EQUAL(newpy,nrows-px);

}
