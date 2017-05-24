#include "../geometry/IShape.h"

namespace nsx {

IShape::IShape()
{
}

IShape::~IShape()
{
    
}

IShape::IShape(const IShape& other)
{
    _lowerBound = other._lowerBound;
    _upperBound = other._upperBound;
}

IShape::IShape(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub) : _lowerBound(lb), _upperBound(ub)
{
    for (unsigned int i=0;i<3;++i)
    {
        if (_lowerBound(i)>_upperBound(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
}

IShape::IShape(const std::initializer_list<double>& lb, const std::initializer_list<double>& ub)
{
    auto it1 = lb.begin();
    auto it2 = ub.begin();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(;it1!=lb.end();it1++,it2++)
    {
        if ((*it1)>(*it2))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
        *(lbit++) = *it1;
        *(ubit++) = *it2;
    }
}

IShape& IShape::operator=(const IShape& other)
{
      if (this != &other)
      {
          _lowerBound = other._lowerBound;
          _upperBound = other._upperBound;
      }
      return *this;

}

bool IShape::contains(const IShape& other) const
{
    for (unsigned int i=0; i<3; ++i)
    {
        if (_lowerBound(i) >= other._lowerBound(i) || _upperBound(i) <= other._upperBound(i))
            return false;
    }
    return true;
}

bool IShape::intercept(const IShape& other) const
{
    for (unsigned int i = 0; i < 3; ++i) {
        if (_upperBound(i) < other._lowerBound(i) || _lowerBound(i) > other._upperBound(i))
            return false;
    }
    return true;
}

void IShape::setBounds(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
{
    for (unsigned int i=0;i<3;++i) {
        if (lb(i)>ub(i))
            throw std::invalid_argument("IShape: upper limit must be > lower limit");
    }
    _lowerBound = lb;
    _upperBound = ub;
}

void IShape::setLower(const Eigen::Vector3d& lb)
{
    for (unsigned int i=0;i<3;++i) {
        if (lb(i)>_upperBound(i))
            throw std::invalid_argument("IShape: upper limit must be > lower limit");
    }
    _lowerBound = lb;
}

void IShape::setUpper(const Eigen::Vector3d& ub)
{
    for (unsigned int i=0;i<3;++i) {
        if (_lowerBound(i)>ub(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
    _upperBound = ub;
}

const Eigen::Vector3d& IShape::getLower() const
{
    return _lowerBound;
}

const Eigen::Vector3d& IShape::getUpper() const
{
    return _upperBound;
}

double IShape::AABBVolume() const
{
    return (_upperBound-_lowerBound).prod();
}

Eigen::Vector3d IShape::getAABBCenter() const
{
    Eigen::Vector3d center((_lowerBound + _upperBound)*0.5);
    return center;
}

Eigen::Vector3d IShape::getAABBExtents() const
{
    Eigen::Vector3d dim(_upperBound - _lowerBound);
    return dim;
}


bool IShape::isInsideAABB(const std::initializer_list<double>& point) const
{

    if (point.size() != 3)
        throw("AABB: invalid point size");

    auto it = point.begin();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(; it!=point.end(); it++,lbit++,ubit++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

bool IShape::isInsideAABB(const Eigen::Vector3d& point) const
{

    auto it = point.data();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(unsigned int i=0; i<3; i++,lbit++,ubit++,it++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

bool IShape::isInsideAABB(const HomVector& point) const
{

    auto it = point.data();
    auto lbit = _lowerBound.data();
    auto ubit = _upperBound.data();

    for(unsigned int i=0; i<3; i++,lbit++,ubit++,it++) {
        if (*it < *lbit || *it > *ubit)
            return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const IShape& shape)
{
    return shape.printSelf(os);
}


std::ostream& IShape::printSelf(std::ostream& os) const
{
      os<<"AABB --> "<<"lower bound: "<<_lowerBound<<" , upper bound: "<<_upperBound;
      return os;
}

void IShape::translateAABB(const Eigen::Vector3d& t)
{
    _lowerBound+=t;
    _upperBound+=t;
}

void IShape::scaleAABB(const Eigen::Vector3d& s)
{
    Eigen::Vector3d center=IShape::getAABBCenter();
    _lowerBound=center+(_lowerBound-center).cwiseProduct(s);
    _upperBound=center+(_upperBound-center).cwiseProduct(s);
}

void IShape::scaleAABB(double s)
{
    Eigen::Vector3d center=IShape::getAABBCenter();
    _lowerBound=center+(_lowerBound-center)*s;
    _upperBound=center+(_upperBound-center)*s;
}

void IShape::rotate(double angle,const Eigen::Vector3d& axis,Direction dir)
{
    if (dir==Direction::CW)
        angle*=-1;
    // Create the quaternion representing this rotation
    double hc=cos(0.5*angle);
    double hs=sin(0.5*angle);
    double norm=axis.norm();
    Eigen::Quaterniond temp(hc,axis(0)*hs/norm,axis(1)*hs/norm,axis(2)*hs/norm);
    rotate(temp.toRotationMatrix());
}

} // end namespace nsx

