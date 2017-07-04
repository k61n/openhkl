#include "AABB.h"
#include "Ellipsoid.h"

namespace nsx {

AABB::AABB() : IShape()
{
}

AABB::~AABB()
{
    
}

AABB::AABB(const AABB& other) : IShape(other)
{
}

AABB::AABB(const Eigen::Vector3d& lb, const Eigen::Vector3d& ub) : IShape(lb,ub)
{
}

AABB::AABB(const std::initializer_list<double>& lb, const std::initializer_list<double>& ub) : IShape(lb,ub)
{
}

AABB& AABB::operator=(const AABB& other)
{
  if (this != &other)
      IShape::operator=(other);
  return *this;
}

IShape* AABB::clone() const
{
    return new AABB(*this);
}

void AABB::rotate(const Eigen::Matrix3d& eigenvectors)
{
    _lowerBound=eigenvectors*_lowerBound;
    _upperBound=eigenvectors*_upperBound;
}

void AABB::scale(double value)
{
    IShape::scaleAABB(value);
}

void AABB::scale(const Eigen::Vector3d& v)
{
    IShape::scaleAABB(v);
}

void AABB::translate(const Eigen::Vector3d& t)
{
    IShape::translateAABB(t);
}

bool AABB::isInside(const HomVector& vector) const
{
    return IShape::isInsideAABB(vector);
}

bool AABB::collide(const IShape& other) const
{
    if (this->intercept(other))
        return other.collide(*this);
    return false;
}

bool AABB::collide(const AABB& other) const
{
    return collideAABBAABB(*this,other);
}

bool AABB::collide(const Ellipsoid& other) const
{
    return collideAABBEllipsoid(*this,other);
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

    return IShape::isInsideAABB(halfvect);
}

bool collideAABBAABB(const AABB& a, const AABB& b)
{
    return a.collide(b);
}

bool collideAABBEllipsoid(const AABB& aabb, const Ellipsoid& ell)
{
    return collideEllipsoidAABB(ell,aabb);
}

} // end namespace nsx

