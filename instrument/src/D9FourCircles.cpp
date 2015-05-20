#include <memory>

#include <Eigen/Dense>

#include "D9FourCircles.h"
#include "Diffractometer.h"
#include "FlatDetector.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "MonochromaticSource.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;


D9FourCircles::D9FourCircles() : Diffractometer("D9 4-circles")
{
}

Diffractometer* D9FourCircles::create(const std::string& name)
{
	return new D9FourCircles(name);
}

Diffractometer* D9FourCircles::clone() const
{
	return new D9FourCircles(*this);
}

D9FourCircles::D9FourCircles(const std::string& name) : Diffractometer(name)
{

	SX::Instrument::FlatDetector* detector = new SX::Instrument::FlatDetector("32x32 flat detector");

    detector->setDistance(488*mm);
    detector->setWidth(64*mm);
    detector->setHeight(64*mm);
    detector->setNPixels(32,32);

    // Attach a gonio to the detector
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("2theta(gamma)",Vector3d(0,0,1),RotAxis::CW);
    g->addTranslation("y-offset",Eigen::Vector3d(0,1,0));
    g->getAxis("y-offset")->setPhysical(false);
    detector->setGonio(g);

    _detector=detector;

    //Sample gonio
    _sample= new Sample("sample");
    std::shared_ptr<Gonio> bl(new Gonio("busing-levy"));
    bl->addRotation("omega",Vector3d(0,0,1),RotAxis::CW);
    bl->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
    bl->addRotation("phi",Vector3d(0,0,1),RotAxis::CW);
    bl->addTranslation("x-sample",Vector3d(1,0,0));
    bl->getAxis("x-sample")->setPhysical(false);
    bl->addTranslation("y-sample",Vector3d(0,1,0));
    bl->getAxis("y-sample")->setPhysical(false);
    bl->addTranslation("z-sample",Vector3d(0,0,1));
    bl->getAxis("z-sample")->setPhysical(false);
    _sample->setGonio(bl);

    _source = new MonochromaticSource("source");

}

D9FourCircles::~D9FourCircles() {
}

std::string D9FourCircles::getType() const
{
	return "D9";
}

} // end namespace Instrument

} // end namespace SX
