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

    Eigen::Vector3d mid_point = from + midt*dir;

    return isInside(mid_point);
}

void AABB::setBounds(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub)
{
    _lowerBound = lb;
    _upperBound = ub;
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

void AABB::scaleAABB(const Eigen::Vector3d& s)
{
    Eigen::Vector3d c = center();
    _lowerBound = c + (_lowerBound-c).cwiseProduct(s);
    _upperBound = c + (_upperBound-c).cwiseProduct(s);
}

void AABB::scaleAABB(double s)
{
    Eigen::Vector3d c = center();
    _lowerBound = c + (_lowerBound-c)*s;
    _upperBound = c + (_upperBound-c)*s;
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

