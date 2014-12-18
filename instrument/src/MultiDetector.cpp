#include "MultiDetector.h"
#include <limits.h>
#include "Gonio.h"

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

MultiDetector::MultiDetector(const MultiDetector& other) : SX::Kernel::Composite<Detector>(other)
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
	return (getMaxCol()-getMinCol());
}

unsigned int MultiDetector::getNRows() const
{
	return (getMaxRow()-getMinRow());
}

int MultiDetector::getMinRow() const
{
	unsigned int minrow=std::numeric_limits<int>::infinity();
	for (auto& detector : _components)
	{
		int row= detector->getMinRow();
		if (row<minrow)
			minrow=row;
	}
	return minrow;
}

int MultiDetector::getMaxRow() const
{
	unsigned int maxrow=0;
	for (auto& detector : _components)
	{
		int row= detector->getMaxRow();
		if (row>maxrow)
			maxrow=row;
	}
	return maxrow;
}

int MultiDetector::getMinCol() const
{
	unsigned int mincol=std::numeric_limits<int>::infinity();
	for (auto& detector : _components)
	{
		int row= detector->getMinCol();
		if (row<mincol)
			mincol=row;
	}
	return mincol;
}

int MultiDetector::getMaxCol() const
{
	unsigned int maxcol=0;
	for (auto& detector : _components)
	{
		int row= detector->getMaxCol();
		if (row>maxcol)
			maxcol=row;
	}
	return maxcol;
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
		{
			if (!detector->hasGonio())
				return detector->getPos(px,py);
			else
				return detector->getGonio()->transform(detector->getPos(px,py));
		}

	}
	throw std::runtime_error("Detector: invalid pixel");
}

unsigned int MultiDetector::getNDetectors() const
{
	return getNComponents();
}

} // end namespace Instrument

} // end namespace SX
