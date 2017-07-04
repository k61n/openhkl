#include <array>

#include "AABB.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"

namespace nsx {

Ellipsoid::Ellipsoid() : IShape()
{
}

Ellipsoid::Ellipsoid(const Ellipsoid& rhs) : IShape()
 {
    //_eigenVal = rhs._eigenVal;
    //_TRSinv = rhs._TRSinv;
    _center = rhs._center;
    _metric = rhs._metric;
    updateAABB();
 }

Ellipsoid& Ellipsoid::operator=(const Ellipsoid& other)
{
    if (this != &other) {
        IShape::operator=(other);
        //_eigenVal = other._eigenVal;
        //_TRSinv = other._TRSinv;
        _metric = other._metric;
        _center = other._center;
        updateAABB();
    }
    return *this;
}

IShape* Ellipsoid::clone() const
{
    return new Ellipsoid(*this);
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& metric): IShape(),
    _center(center), _metric(metric)
{
    updateAABB();
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& eigenvalues, const Eigen::Matrix3d& eigenvectors)
: IShape()
{
    Eigen::Matrix3d D = Eigen::Matrix3d::Identity();
    for (auto i = 0; i < 3; ++i) {
        D(i,i) = 1.0 / (eigenvalues[i] * eigenvalues[i]);
    }
    _metric = eigenvectors * D * eigenvectors.transpose();
    _center = center;
    updateAABB();
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, double radius)
: IShape()
{
    _metric = Eigen::Matrix3d::Identity()/(radius*radius);
    _center = center;
    updateAABB();
}

bool Ellipsoid::collide(const IShape& other) const
{
    if (this->intercept(other)) {
        return other.collide(*this);
    }
    return false;
}

bool Ellipsoid::collide(const AABB& aabb) const
{
    return collideEllipsoidAABB(*this,aabb);
}

bool Ellipsoid::collide(const Ellipsoid& other) const
{ 
    const auto& A = homogeneousMatrix();
    const auto& B = other.homogeneousMatrix();
    Eigen::Matrix4d M = A.inverse() * B;

    Eigen::ComplexEigenSolver<Eigen::Matrix4d> solver(M);
    const auto& val = solver.eigenvalues();

    // One of the root is always positive.
    // Check whether two of the roots are negative and distinct, in which case the Ellipse do not collide.
    int count=0;
    double sol[2];
    if (std::fabs(imag(val(0)))< 1e-5 && real(val(0))<0) {
        sol[count++]=real(val(0));
    }
    if (std::fabs(imag(val(1)))< 1e-5 && real(val(1))<0) {
        sol[count++]=real(val(1));
    }
    if (std::fabs(imag(val(2)))< 1e-5 && real(val(2))<0) {
        sol[count++]=real(val(2));
    }
    if (std::fabs(imag(val(3)))< 1e-5 && real(val(3))<0) {
        sol[count++]=real(val(3));
    }
    return (!(count==2 && std::fabs(sol[0]-sol[1])>1e-5));
}


void Ellipsoid::rotate(const Eigen::Matrix3d& U)
{
    _metric = U * _metric * U.transpose();
    // Update the bounds of the AABB
    updateAABB();
}

void Ellipsoid::scale(double value)
{
    _metric /= value*value;
    this->scaleAABB(value);
}

void Ellipsoid::translate(const Eigen::Vector3d& t)
{
    _center += t;    
    this->translateAABB(t);
}

bool Ellipsoid::isInside(const HomVector& point) const
{
    const double s = 1.0 / point(3);
    Eigen::Vector3d u(point(0)*s, point(1)*s, point(2)*s);
    u -= _center;
    return u.transpose() * _metric * u < 1.0;
}

bool Ellipsoid::isInside(const Eigen::Vector3d& point) const
{
    Eigen::Vector3d u = point - _center;
    return u.transpose() * _metric * u <= 1.0;
}

const HomMatrix& Ellipsoid::getInverseTransformation() const
{
    // new implementation
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_metric);

    auto&& eigenvalues = solver.eigenvalues();
    auto&& eigenvectors = solver.eigenvectors();

    // Define the inverse scale matrix from the eigenvalues
    Eigen::DiagonalMatrix<double,4> Sinv;
    static Eigen::Matrix4d TRSinv;
    for (unsigned int i = 0; i < 3; ++i) {
        Sinv.diagonal()[i] = std::sqrt(eigenvalues[i]);
    }
    Sinv.diagonal()[3]=1.0;

    // Now prepare the R^-1.T^-1 (rotation,translation)
    TRSinv = HomMatrix::Constant(0.0);
    TRSinv(3,3) = 1.0;

    for (unsigned int i = 0; i < 3; ++i) {
        TRSinv.block(i,0,1,3) = eigenvectors.col(i).transpose().normalized();
    }

    // The translation part of the inverse transforation matrix is afected by rotation
    // (see https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_homog%C3%A8nes)
    TRSinv.block(0,3,3,1) = -TRSinv.block(0,0,3,3)*_center;

    // Finally compute (TRS)^-1 by left-multiplying (TR)^-1 by S^-1
    TRSinv = Sinv*TRSinv;

    return TRSinv;
}

void Ellipsoid::updateAABB()
{
    const auto& B = _metric.inverse();
    Eigen::Vector3d a;

    for (auto i = 0; i < 3; ++i) {
        a(i) = std::sqrt(B(i,i));
    }
    _lowerBound = _center - a;
    _upperBound = _center + a;
}

bool Ellipsoid::rayIntersect(const Eigen::Vector3d& from, const Eigen::Vector3d& dir, double& t1, double& t2) const
{
    auto&& d = dir;
    auto&& a = from;
    auto&& c = _center;
    auto&& ac = a-c;
    auto&& A = _metric;

    const double alpha = d.dot(A*d);
    const double beta = 2*d.dot(A*ac);
    const double gamma = ac.dot(A*ac)-1.0;

    const double discr = beta*beta -4*alpha*gamma;

    if (discr < 0) {
        return false;
    }

    const double delta = std::sqrt(discr);

    t1 = (-beta-delta)/(2*alpha);
    t2 = (-beta+delta)/(2*alpha);

    return !(t1 < 0 && t2 < 0);
}

Eigen::Matrix3d Ellipsoid::getRSinv() const
{
    Eigen::Matrix3d A;
    Eigen::Matrix4d TRSinv = getInverseTransformation();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            A(i, j) = TRSinv(i, j);
        }
    }
    return A;
}

const Eigen::Vector3d& Ellipsoid::center() const
{
    return _center;
}

const Eigen::Matrix3d& Ellipsoid::metric() const
{
    return _metric;
}

bool collideEllipsoidAABB(const Ellipsoid& ellipsoid, const AABB& aabb)
{
    // Cheap preliminary test. Is the center inside the ellipsoid ? If so, then they intersect.
    if (ellipsoid.isInside(aabb.getAABBCenter())) {
        return true;
    }

    auto&& Ainv = ellipsoid.metric().inverse();
    auto&& x0 = ellipsoid.center();

    const auto& lb = aabb.getLower();
    const auto& ub = aabb.getUpper();

    const std::array<Eigen::Vector3d,3> normals = {Eigen::Vector3d(1,0,0),Eigen::Vector3d(0,1,0),Eigen::Vector3d(0,0,1)};

    for (auto i=0; i< 3; ++i) {

        std::array<double,2> minmax = {lb[i],ub[i]};

        const auto& n = normals[i];

        double nt_Ainv_n = n.transpose() * Ainv * normals[i];
        auto Ainv_n = Ainv * n;
        auto nt_x0 = n.transpose()*x0;

        for (auto j=0; j<2; ++j) {
            Eigen::Vector3d point = x0 + ((minmax[j]-nt_x0)/nt_Ainv_n)*Ainv_n;
            if (ellipsoid.isInside(point) && aabb.isInsideAABB(point))
                return true;
        }
    }

    return false;
}

double Ellipsoid::getVolume() const
{
    static constexpr double c = 4.0*M_PI / 3.0;
    return c * std::pow(_metric.determinant(), -0.5);
}

Eigen::Matrix4d Ellipsoid::homogeneousMatrix() const
{
    Eigen::Matrix4d Q = Eigen::Matrix4d::Zero();
    Q.block<3,3>(0, 0) = _metric;
    Q.block<3,1>(0, 3) = -_metric * _center;
    Q.block<1,3>(3,0) = (-_metric * _center).transpose();
    Q(3,3) = _center.dot(_metric*_center)-1.0;
    return Q;
}

Eigen::Vector3d Ellipsoid::eigenvalues() const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_metric);
    Eigen::Vector3d vals = solver.eigenvalues();

    for (auto i = 0; i < 3; ++i) {
        vals(i) = 1.0 / std::sqrt(vals(i));
    }
    return vals;
}

} // end namespace nsx
