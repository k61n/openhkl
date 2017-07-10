#include <array>

#include "AABB.h"
#include "Ellipsoid.h"
#include "GeometryTypes.h"

namespace nsx {

Ellipsoid::Ellipsoid()
{
}

Ellipsoid::Ellipsoid(const Ellipsoid& other)
 {
    _center = other._center;
    _metric = other._metric;
    _inverseMetric = other._inverseMetric;
 }

Ellipsoid& Ellipsoid::operator=(const Ellipsoid& other)
{
    if (this != &other) {
        _metric = other._metric;
        _inverseMetric = other._inverseMetric;
        _center = other._center;
    }
    return *this;
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& metric)
: _center(center),
  _metric(metric),
  _inverseMetric(metric.inverse())
{

}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Vector3d& radii, const Eigen::Matrix3d& axes)
{
    Eigen::Matrix3d D = Eigen::Matrix3d::Identity();
    for (auto i = 0; i < 3; ++i) {
        D(i,i) = 1.0 / (radii[i] * radii[i]);
    }

    // By definition, we have A.U = U.D where A is the metric tensor, U is the matric of columned eigen-vectors and D the diagonal matrix of corresponding eigen values
    _metric = axes * D * axes.transpose();
    _inverseMetric = _metric.inverse();
    _center = center;
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, double radius)
{
    _metric = Eigen::Matrix3d::Identity()/(radius*radius);
    _inverseMetric = _metric.inverse();
    _center = center;
}

bool Ellipsoid::collide(const AABB& aabb) const
{
    const std::vector<Eigen::Vector3d> normals = {
        Eigen::Vector3d(1,0,0),
        Eigen::Vector3d(0,1,0),
        Eigen::Vector3d(0,0,1),
    };

    const auto& lb = aabb.lower();
    const auto& ub = aabb.upper();

    for (auto&& n: normals) {
        
        const Eigen::Vector3d dx = _inverseMetric*n;
        const double dl = lb.dot(n);
        const double du = ub.dot(n);
        const double nAn = n.dot(dx);
        const double den = std::sqrt(nAn);

        
        const Eigen::Vector3d x1 = _center + dx/den;
        const Eigen::Vector3d x2 = _center - dx/den;

        const double e1 = x1.dot(n);
        const double e2 = x2.dot(n);

        const double min_d = e1 < e2 ? e1 : e2;
        const double max_d = e1 > e2 ? e1 : e2;

        if (max_d < dl) {
            return false;
        }
        if (min_d > du) {
            return false;
        }
    }
    return true;
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
    double sol[4];
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
}

void Ellipsoid::scale(double value)
{
    _metric /= value*value;
    _inverseMetric *= value*value;
}

void Ellipsoid::translate(const Eigen::Vector3d& t)
{
    _center += t;  
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

double Ellipsoid::volume() const
{
    static constexpr double c = 4.0*M_PI / 3.0;
    return c * std::pow(_metric.determinant(), -0.5);
}

const AABB Ellipsoid::aabb() const
{
    Eigen::Vector3d a;

    for (auto i = 0; i < 3; ++i) {
        a(i) = std::sqrt(_inverseMetric(i,i));
    }
    Eigen::Vector3d lb(_center - a);
    Eigen::Vector3d ub(_center + a);

    return AABB(lb, ub);
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

Eigen::Matrix4d Ellipsoid::homogeneousMatrixInverse() const
{
    Eigen::Matrix4d Q = Eigen::Matrix4d::Zero();
    Q.block<3,3>(0, 0) = _inverseMetric - _center*_center.transpose();
    Q.block<3,1>(0, 3) = - _center;
    Q.block<1,3>(3,0) = - _center.transpose();
    Q(3,3) = -1.0;
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
