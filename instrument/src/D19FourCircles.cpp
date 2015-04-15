#include <memory>

#include <Eigen/Dense>

#include "D19FourCircles.h"
#include "CylindricalDetector.h"
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

D19FourCircles::D19FourCircles() : Diffractometer("D19 4-circles")
{
}

Diffractometer* D19FourCircles::create(const std::string& name)
{
	return new D19FourCircles(name);
}

Diffractometer* D19FourCircles::clone() const
{
	return new D19FourCircles(*this);
}

D19FourCircles::D19FourCircles(const std::string& name) : Diffractometer(name)
{

    SX::Instrument::CylindricalDetector* detector = new SX::Instrument::CylindricalDetector("640x256 cylindrical detector");

    detector->setDistance(764*mm);
    detector->setAngularWidth(120.0*deg);
    detector->setHeight(398.4375*mm);
    detector->setNPixels(640,256);

    // Define the goniometer bound to the detector
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("2theta(gamma)",Vector3d(0,0,1),RotAxis::CW);
    g->addTranslation("x-offset",Vector3d(1,0,0));
    g->getAxis("x-offset")->setPhysical(false);
    g->addTranslation("y-offset",Vector3d(0,1,0));
    g->getAxis("y-offset")->setPhysical(false);
    g->addTranslation("z-offset",Vector3d(0,0,1));
    g->getAxis("z-offset")->setPhysical(false);
    detector->setGonio(g);

    _detector = detector;

    // Define the goniometer bound to the sample
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

D19FourCircles::~D19FourCircles() {
}

std::string D19FourCircles::getType() const
{
	return "D19";
}

} // end namespace Instrument

} // end namespace SX
