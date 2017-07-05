#include "AABB.h"
#include "Ellipsoid.h"

namespace nsx {

AABB::AABB()
{
}

AABB::~AABB()
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

void AABB::rotate(const Eigen::Matrix3d& eigenvectors)
{
    _lowerBound=eigenvectors*_lowerBound;
    _upperBound=eigenvectors*_upperBound;
}

void AABB::scale(double value)
{
    scaleAABB(value);
}

void AABB::scale(const Eigen::Vector3d& v)
{
    scaleAABB(v);
}

void AABB::translate(const Eigen::Vector3d& t)
{
    translateAABB(t);
}

bool AABB::isInside(const HomVector& vector) const
{
    return isInsideAABB(vector);
}

bool AABB::collide(const AABB& other) const
{
    for (unsigned int i = 0; i < 3; ++i) {
        if (_upperBound(i) < other._lowerBound(i) || _lowerBound(i) > other._upperBound(i))
            return false;
    }
    return true;
}

bool AABB::intercept(const Ellipsoid& other) const
{
    for (unsigned int i = 0; i < 3; ++i) {
        if (_upperBound(i) < other._lowerBound(i) || _lowerBound(i) > other._upperBound(i))
            return false;
    }
    return true;
}

bool AABB::collide(const Ellipsoid& other) const
{
    return other.collide(*this);
}

bool AABB::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{
    // Adapted from
    // See http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
    // to get any values of t, negative or positive

    // Check first the case of ray // to one the box axis
    for (unsigned int i=0;i<3;++i)
    {
        if (std::abs(dir[i])<1.0e-10)
        {
            if (from[i] < _lowerBound[i] || from[i] > _upperBound[i])
                return false;
        }
    }

    // Keep track of t-values for every direction
    std::vector<double> tvalues;
    int nSlabs = 6;
    tvalues.reserve(nSlabs);

    // We test slabs in every direction
    for (unsigned int i=0; i<3; i++)
    {
        double invdir=1.0/dir[i];
        tvalues.push_back((_lowerBound[i] - from[i])*invdir);
        tvalues.push_back((_upperBound[i] - from[i])*invdir);
    }

    std::sort(tvalues.begin(),tvalues.end());

    t1 = tvalues[nSlabs/2 - 1];
    t2 = tvalues[nSlabs/2];

    double midt = (t1+t2)/2.0;

    Eigen::Vector3d halfvect = from + midt*dir;

    return isInsideAABB(halfvect);
}

void AABB::setBounds(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
{
    for (unsigned int i=0;i<3;++i) {
        if (lb(i)>ub(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
    _lowerBound = lb;
    _upperBound = ub;
}

void AABB::setLower(const Eigen::Vector3d& lb)
{
    for (unsigned int i=0;i<3;++i) {
        if (lb(i)>_upperBound(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
    _lowerBound = lb;
}

void AABB::setUpper(const Eigen::Vector3d& ub)
{
    for (unsigned int i=0;i<3;++i) {
        if (_lowerBound(i)>ub(i))
            throw std::invalid_argument("AABB: upper limit must be > lower limit");
    }
    _upperBound = ub;
}

const Eigen::Vector3d& AABB::getLower() const
{
    return _lowerBound;
}

const Eigen::Vector3d& AABB::getUpper() const
{
    return _upperBound;
}

double AABB::AABBVolume() const
{
    return (_upperBound-_lowerBound).prod();
}

Eigen::Vector3d AABB::getAABBCenter() const
{
    Eigen::Vector3d center((_lowerBound + _upperBound)*0.5);
    return center;
}

Eigen::Vector3d AABB::getAABBExtents() const
{
    Eigen::Vector3d dim(_upperBound - _lowerBound);
    return dim;
}


bool AABB::isInsideAABB(const std::initializer_list<double>& point) const
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

bool AABB::isInsideAABB(const Eigen::Vector3d& point) const
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

bool AABB::isInsideAABB(const HomVector& point) const
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

void AABB::translateAABB(const Eigen::Vector3d& t)
{
    _lowerBound+=t;
    _upperBound+=t;
}

void AABB::scaleAABB(const Eigen::Vector3d& s)
{
    Eigen::Vector3d center=getAABBCenter();
    _lowerBound=center+(_lowerBound-center).cwiseProduct(s);
    _upperBound=center+(_upperBound-center).cwiseProduct(s);
}

void AABB::scaleAABB(double s)
{
    Eigen::Vector3d center=getAABBCenter();
    _lowerBound=center+(_lowerBound-center)*s;
    _upperBound=center+(_upperBound-center)*s;
}

void AABB::rotate(double angle,const Eigen::Vector3d& axis,Direction dir)
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
      os<<"AABB --> "<<"lower bound: "<<_lowerBound<<" , upper bound: "<<_upperBound;
      return os;
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb)
{
    return aabb.printSelf(os);
}

} // end namespace nsx

