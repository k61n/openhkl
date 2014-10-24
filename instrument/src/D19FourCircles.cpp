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

    _detector = new SX::Instrument::CylindricalDetector("detector");

    _detector->setDistance(764*mm);
    _detector->setWidthAngle(119.8*deg);
    _detector->setHeight(40.0*cm);
    _detector->setNPixels(640,256);

    _detector->setDataMapping([]
                               (double x, double y, double& newx, double& newy)
                               {
									newx = 640 - x;
									newy = 256 - y;
                               });

    // Define the goniometer bound to the detector
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("2theta(gamma)",Vector3d(0,0,1),RotAxis::CW);
    g->addTranslation("y-offset",Vector3d(0,1,0));
    g->getAxis("y-offset")->setPhysical(false);
    _detector->setGonio(g);

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
