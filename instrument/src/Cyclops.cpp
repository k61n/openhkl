#include <memory>

#include <Eigen/Dense>

#include "Cyclops.h"
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

Cyclops::Cyclops() : Diffractometer("Cyclops")
{
}

Diffractometer* Cyclops::create(const std::string& name)
{
	return new Cyclops(name);
}

Diffractometer* Cyclops::clone() const
{
	return new Cyclops(*this);
}

Cyclops::Cyclops(const std::string& name) : Diffractometer(name)
{

    _detector = new SX::Instrument::FlatDetector("Flat");

    _detector->setDistance(488*mm);
    _detector->setWidth(64*mm);
    _detector->setHeight(64*mm);
    _detector->setNPixels(7680,2400);


    //Sample gonio
    _sample= new Sample("sample");
    std::shared_ptr<Gonio> bl(new Gonio("Busing-Levy"));
    bl->addRotation("omega",Vector3d(0,0,1),RotAxis::CCW);
    _sample->setGonio(bl);

    _source = new Source("source");

}

Cyclops::~Cyclops() {
}

std::string Cyclops::getType() const
{
	return "Cyclops";
}

} // end namespace Instrument

} // end namespace SX
