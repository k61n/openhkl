#include <memory>

#include <Eigen/Dense>

#include "D3.h"
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

D3::D3() : Diffractometer("D3 4-circles")
{
}

Diffractometer* D3::create(const std::string& name)
{
	return new D3(name);
}

Diffractometer* D3::clone() const
{
	return new D3(*this);
}

D3::D3(const std::string& name) : Diffractometer(name)
{

    SX::Instrument::FlatDetector* detector = new SX::Instrument::FlatDetector("32x32 flat detector");

    detector->setDistance(488*mm);
    detector->setWidth(80*mm);
    detector->setHeight(80*mm);
    detector->setNPixels(32,32);

    // Attach a gonio to the detector
    std::shared_ptr<Gonio> g(new Gonio("gamma-arm"));
    g->addRotation("2theta(gamma)",Vector3d(0,0,1),RotAxis::CCW);
    detector->setGonio(g);

    _detector = detector;

    //Sample gonio
    _sample= new Sample("sample");
    std::shared_ptr<Gonio> bl(new Gonio("Busing-Levy"));
    bl->addRotation("chi",Vector3d(0,1,0),RotAxis::CCW);
    bl->addRotation("phi",Vector3d(0,0,1),RotAxis::CCW);

    _sample->setGonio(bl);

    _source = new Source("source");

}

D3::~D3() {
}

std::string D3::getType() const
{
	return "D3";
}

} // end namespace Instrument

} // end namespace SX
