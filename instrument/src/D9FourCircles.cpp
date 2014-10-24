#include <memory>

#include <Eigen/Dense>

#include "D9FourCircles.h"
#include "Diffractometer.h"
#include "FlatDetector.h"
#include "Gonio.h"
#include "RotAxis.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;

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

    _detector = new SX::Instrument::FlatDetector("detector");

    _detector->setDistance(488*mm);
    _detector->setWidth(64*mm);
    _detector->setHeight(64*mm);
    _detector->setNPixels(32,32);

    _detector->setDataMapping([]
                               (double x, double y, double& newx, double& newy)
                               {
									newx = 32 - x;
									newy = 32 - y;
                               });

    // Attach a gonio to the detector
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("2theta(gamma)",Vector3d(0,0,1),RotAxis::CW);
    g->addTranslation("y-offset",Eigen::Vector3d(0,1,0));
    g->getAxis("y-offset")->setPhysical(false);
    _detector->setGonio(g);

    //Sample gonio
    _sample= new Sample("sample");
    std::shared_ptr<Gonio> bl(new Gonio("Busing-Levy"));
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

    _source = new Source("source");

}

D9FourCircles::~D9FourCircles() {
}

std::string D9FourCircles::getType() const
{
	return "D9";
}

} // end namespace Instrument

} // end namespace SX
