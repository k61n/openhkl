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

MultiDetector::~MultiDetector()
{
}

Detector* MultiDetector::clone() const
{
	return new MultiDetector(*this);
}

unsigned int MultiDetector::getNCols() const
{
	unsigned int nCols=0;
	for (auto& detector : _components)
		nCols += detector->getNCols();
	return nCols;
}

unsigned int MultiDetector::getNRows() const
{
	unsigned int nRows=0;
	for (auto& detector : _components)
		nRows += detector->getNRows();
	return nRows;
}

bool MultiDetector::hasPixel(double px, double py) const
{
	for (auto& detector : _components)
		if (detector->hasPixel(px,py))
			return true;
	return false;
}

double MultiDetector::getHeight() const
{
	double height=0.0;
	for (auto& detector : _components)
		height += detector->getHeight();
	return height;
}

double MultiDetector::getWidth() const
{
	double width=0.0;
	for (auto& detector : _components)
		width += detector->getWidth();
	return width;
}

double MultiDetector::getAngularHeight() const
{
	double angularHeigth=0.0;
	for (auto& detector : _components)
		angularHeigth += detector->getAngularHeight();
	return angularHeigth;
}

double MultiDetector::getAngularWidth() const
{
	double angularWidth=0.0;
	for (auto& detector : _components)
		angularWidth += detector->getAngularWidth();
	return angularWidth;
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

unsigned int MultiDetector::getNDetectors() const
{
	return getNComponents();
}

} // end namespace Instrument

} // end namespace SX
