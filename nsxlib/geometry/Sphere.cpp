#include "Sphere.h"

namespace nsx {

Sphere::Sphere(const Sphere& other) : IShape(other)
{
    _center = other._center;
    _radius = other._radius;
    updateAABB();
}

Sphere::Sphere(const Eigen::Vector3d& center, double radius) : IShape(), _center(center), _radius(radius)
{
    updateAABB();
}

Sphere& Sphere::operator=(const Sphere& other)
{
    if (this != &other)
    {
        IShape::operator=(other);
        _center = other._center;
        _radius = other._radius;
        updateAABB();
    }
    return *this;
}

IShape* Sphere::clone() const
{
    return new Sphere(*this);
}

bool Sphere::collide(const IShape& other) const
{
    if (this->intercept(other))
        return other.collide(*this);
    return false;
}

bool Sphere::collide(const AABB& aabb) const
{
    return collideSphereAABB(*this,aabb);
}

bool Sphere::collide(const Ellipsoid& ell) const
{
    return collideSphereEllipsoid(*this,ell);
}

bool Sphere::collide(const OBB& obb) const
{
    return collideSphereOBB(*this,obb);
}

bool Sphere::collide(const Sphere& other) const
{
    return collideSphereSphere(*this,other);
}

const Eigen::Vector3d& Sphere::getCenter() const
{
    return _center;
}

double Sphere::getRadius() const
{
    return _radius;
}

HomMatrix Sphere::getInverseTransformation() const
{
    HomMatrix mat=HomMatrix::Constant(0.0);
    mat(3,3)=1.0;
    double invRadius = 1.0/_radius;
    for (unsigned int i=0;i<4;++i)
        mat(i,i)=invRadius;
    mat.block(0,3,3,1)=-_center*invRadius;

    return mat;
}

bool Sphere::isInside(const HomVector& point) const
{
    Eigen::Vector3d diff = point.segment(0,D) - _center;
    return (diff.squaredNorm() < (_radius*_radius));
}

void Sphere::rotate(const Eigen::Matrix3d& eigenvectors)
{
}

void Sphere::scale(double value)
{
    _radius *= value;
    updateAABB();
}

void Sphere::translate(const Eigen::Vector3d& t)
{
    _center += t;
    updateAABB();
}

void Sphere::updateAABB()
{
    // Update the upper and lower bound of the AABB
    _lowerBound=_center.array()-_radius;
    _upperBound=_center.array()+_radius;
}

bool Sphere::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{
    // The intersection are found by solving the equation (x-a)^2 + (y-b)^2 + (z-c)^2 = R^2 with
    // p=(x,y,z). Using p=p0+t*dir in this equation provides an equation of the form a*t^2 + b*t + c = 0.

    double a = dir.squaredNorm();
    double b = 2.0*(from-_center).dot(dir);
    double c = _center.squaredNorm() + from.squaredNorm() - 2.0*_center.dot(from) - _radius*_radius;

    // Solve the 2nd degree equation
    double delta = b*b - 4.0*a*c;
    if (delta < 0) {
        return false;
    }
    double sdelta = sqrt(delta);
    t1 = 0.5*(-b - sdelta)/a;
    t2 = 0.5*(-b + sdelta)/a;
    return true;
}

bool collideSphereAABB(const Sphere& sphere, const AABB& aabb)
{
    OBB obb(aabb);
    return collideSphereOBB(sphere,obb);
}

bool collideSphereSphere(const Sphere& a, const Sphere& b)
{
    Eigen::Vector3d diff=b.getCenter()-a.getCenter();
    T sumRadii=a.getRadius()+b.getRadius();
    return (diff.squaredNorm()<(sumRadii*sumRadii));
}

bool collideSphereEllipsoid(const Sphere& s, const Ellipsoid& eB)
{
    return collideEllipsoidSphere(eB,s);
}

bool collideSphereOBB(const Sphere& s, const OBB& obb)
{
    Eigen::Vector3d scale=Eigen::Vector3d::Constant(s.getRadius());
    Eigen::Matrix3d rot=Eigen::Matrix3d::Identity();
    Ellipsoid ell(s.getCenter(),scale,rot);
    return collideEllipsoidOBB(ell,obb);
}

} // end namespace nsx

