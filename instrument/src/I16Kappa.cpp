#include <memory>

#include <Eigen/Dense>

#include "I16Kappa.h"
#include "Diffractometer.h"
#include "FlatDetector.h"
#include "Gonio.h"
#include "Axis.h"
#include "RotAxis.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Units;

I16Kappa::I16Kappa() : Diffractometer("I16Kappa")
{
}

Diffractometer* I16Kappa::create(const std::string& name)
{
	return new I16Kappa(name);
}

Diffractometer* I16Kappa::clone() const
{
	return new I16Kappa(*this);
}

I16Kappa::I16Kappa(const std::string& name) : Diffractometer(name)
{

    SX::Instrument::FlatDetector* detector = new SX::Instrument::FlatDetector("Pilatus 100K");

    detector->setDistance(488*mm);
    detector->setWidth(64*mm);
    detector->setHeight(64*mm);
    detector->setNPixels(487,195);
    std::shared_ptr<Gonio> g(new Gonio("delta-arm"));
    g->addRotation("delta",Vector3d(1,0,0),RotAxis::CCW);
    g->addRotation("gam",Vector3d(0,0,1),RotAxis::CCW);
    detector->setGonio(g);

    _detector=detector;

	  //Sample gonio
	  _sample= new Sample("sample");
	  std::shared_ptr<Gonio> bl(new Gonio("busing-levy"));
	  bl->addRotation("phi",Vector3d(1,0,0),RotAxis::CCW);
	  bl->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
	  bl->addRotation("eta",Vector3d(1,0,0),RotAxis::CCW);
	  bl->addRotation("mu",Vector3d(0,0,1),RotAxis::CCW);
	  _sample->setGonio(bl);

    _source = new Source("source");

}

I16Kappa::~I16Kappa() {
}

std::string I16Kappa::getType() const
{
	return "I16Kappa";
}

} // end namespace Instrument

} // end namespace SX
