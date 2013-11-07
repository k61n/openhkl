#include "AABBObject.h"

namespace SX
{

namespace Geometry
{

AABBObject::AABBObject(): _lowerBound(0.0,0.0,0.0), _upperBound(1.0,1.0,1.0)
{
}

AABBObject::AABBObject(const V3D& lBound, const V3D& uBound): _lowerBound(lBound), _upperBound(uBound)
{
}

AABBObject::AABBObject(const AABBObject& other)
{
	_lowerBound = other._lowerBound;
	_upperBound = other._upperBound;
}

AABBObject& AABBObject::operator=(const AABBObject& other)
{
  if (this != &other)
  {
	  _lowerBound = other._lowerBound;
	  _upperBound = other._upperBound;
  }
  return *this;
}

const V3D& AABBObject::getLowerBound() const
{
	return _lowerBound;
}

V3D& AABBObject::getLowerBound()
{
	return _lowerBound;
}

const V3D& AABBObject::getUpperBound() const
{
	return _upperBound;
}

V3D& AABBObject::getUpperBound()
{
	return _upperBound;
}

V3D AABBObject::getCenter() const
{
	return (_lowerBound + _upperBound)/2.0;
}

std::vector<V3D> AABBObject::getExtents() const
{
	std::vector<V3D> extents;
	extents.reserve(2);

	V3D center(getCenter());

	extents.push_back(_upperBound-center);
	extents.push_back(_lowerBound-center);

	return extents;
}



}

}
