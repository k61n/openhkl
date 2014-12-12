#include "MultiDetector.h"

namespace SX
{

namespace Instrument
{

Detector* MultiDetector::create(const std::string& name)
{
	return new MultiDetector(name);
}

MultiDetector::MultiDetector() : SX::Kernel::Composite<Detector>()
{
}

MultiDetector::MultiDetector(const MultiDetector& other) : SX::Kernel::Composite<Detector>()
{
}

MultiDetector::MultiDetector(const std::string& name) : SX::Kernel::Composite<Detector>()
{
}

Eigen::Vector3d MultiDetector::getPos(double px, double py) const
{

	for (auto& detector : _components)
	{
		if (detector->hasPixel(px,py))
			return detector->getPos(px,py);
	}
	throw std::runtime_error("Detector: invalid pixel");
}

MultiDetector::~MultiDetector()
{
}

Detector* MultiDetector::clone() const
{
	return new MultiDetector(*this);
}

void MultiDetector::setWidthAngle(double wangle)
{
	_widthAngle = wangle;
	_width=0.0;
	for (auto& detector : _components)
	{
		detector->setWidthAngle(wangle);
		_width += detector->getWidthAngle();
	}
}

void MultiDetector::setHeightAngle(double hangle)
{
	_heightAngle = hangle;
	_height=0.0;
	for (auto& detector : _components)
	{
		detector->setHeightAngle(hangle);
		_height += detector->getHeightAngle();
	}
}

void MultiDetector::setAngularRange(double w, double h)
{
	setWidthAngle(w);
	setHeightAngle(h);
}

unsigned int MultiDetector::getNDetectors() const
{
	return getNComponents();
}

} // end namespace Instrument

} // end namespace SX
