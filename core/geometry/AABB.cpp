#include "AABB.h"
#include "Ellipsoid.h"

namespace nsx {

AABB::AABB()
{
}

AABB::AABB(const AABB& other)
{
    _lowerBound = other._lowerBound;
    _upperBound = other._upperBound;
}

AABB::AABB(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
: _lowerBound(lb),
  _upperBound(ub)
{
}

AABB& AABB::operator=(const AABB& other)
{
  if (this != &other) {
      _lowerBound = other._lowerBound;
      _upperBound = other._upperBound;

  }
  return *this;
}

bool AABB::isInside(const Eigen::Vector3d& point) const
{
    for(auto i=0; i<3; ++i) {
        if (point(i) < _lowerBound(i) || point(i) > _upperBound(i))
            return false;
    }

    return true;
}

bool AABB::collide(const AABB& aabb) const
{
    for (unsigned int i = 0; i < 3; ++i) {
        if (_upperBound(i) < aabb._lowerBound(i) || _lowerBound(i) > aabb._upperBound(i))
            return false;
    }
    return true;
}

bool AABB::collide(const Ellipsoid& ellipsoid) const
{
    return ellipsoid.collide(*this);
}

void AABB::setLower(const Eigen::Vector3d& lower)
{
    _lowerBound = lower;
}

void AABB::setUpper(const Eigen::Vector3d& upper)
{
    _upperBound = upper;
}

const Eigen::Vector3d& AABB::lower() const
{
    return _lowerBound;
}

const Eigen::Vector3d& AABB::upper() const
{
    return _upperBound;
}

Eigen::Vector3d AABB::center() const
{
    return (_lowerBound + _upperBound)*0.5;
}

Eigen::Vector3d AABB::extents() const
{
    return _upperBound - _lowerBound;
}

void AABB::translate(const Eigen::Vector3d& t)
{
    _lowerBound+=t;
    _upperBound+=t;
}

bool AABB::contains(const AABB& other) const
{
    for (unsigned int i=0; i<3; ++i)
    {
        if (_lowerBound(i) >= other._lowerBound(i) || _upperBound(i) <= other._upperBound(i))
            return false;
    }
    return true;
}

std::ostream& AABB::printSelf(std::ostream& os) const
{
      os<<"AABB --> "<<"lower bound: "<<_lowerBound.transpose()<<" , upper bound: "<<_upperBound.transpose();
      return os;
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb)
{
    return aabb.printSelf(os);
}

} // end namespace nsx

