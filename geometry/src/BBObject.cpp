#include "BBObject.h"

namespace SX
{

namespace Geometry
{

BBObject::BBObject(): _lowerBound(0.0,0.0,0.0), _upperBound(1.0,1.0,1.0)
{
}

BBObject::BBObject(const V3D& lBound, const V3D& uBound): _lowerBound(lBound), _upperBound(uBound)
{
}

BBObject::BBObject(const BBObject& other)
{
	_lowerBound =other._lowerBound;
	_upperBound =other._upperBound;
}

BBObject& BBObject::operator=(const BBObject& other)
{
  if (this != &other)
  {
	  _lowerBound = other._lowerBound;
	  _upperBound = other._upperBound;
  }
  return *this;
}

const V3D& BBObject::getLowerBound() const
{
	return _lowerBound;
}

V3D& BBObject::getLowerBound() const
{
	return _lowerBound;
}

const V3D& BBObject::getUpperBound() const
{
	return _upperBound;
}

V3D& BBObject::getUpperBound() const
{
	return _upperBound;
}

V3D BBObject::getCenter() const
{
	V3D center((_lowerBound + _upperBound)/2.0);

	return center;
}

std::vector<V3D> BBObject::getExtents() const
{
	std::vector<V3D> axis;
	axis.reserve(2);

	V3D center(getCenter());

	axis.push_back(_upperBound-center);
	axis.push_back(_lowerBound-center);

	return axis;
}


}

}
