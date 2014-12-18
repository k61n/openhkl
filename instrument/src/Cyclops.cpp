#include <memory>

#include <Eigen/Dense>

#include "Cyclops.h"
#include "MultiDetector.h"
#include "MonoDetector.h"
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

	SX::Instrument::MultiDetector* det=new SX::Instrument::MultiDetector("");
	SX::Instrument::FlatDetector panel;
	panel.setWidth(166*mm);
	panel.setHeight(415*mm);
	panel.setNPixels(960,2400);
	panel.setDistance(200.38*mm);

	for (int i=0;i<8;++i)
	{
		SX::Instrument::MonoDetector* current=new SX::Instrument::FlatDetector(panel);
		current->setOrigin(960*i,0);
		std::shared_ptr<Gonio> g(new Gonio(""));
		SX::Instrument::Axis* a=g->addRotation("",Vector3d(0,0,1),RotAxis::CW);
		a->setPhysical(false);
		a->setOffset(-180*deg+i*45.0*deg);
		current->setGonio(g);
		det->add(current);
		std::cout<<"LOCAL "<<current->getPos(480+960*i,1200)<<" "<<current->getDistance()<<std::endl;
	}

	_detector=det;

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
