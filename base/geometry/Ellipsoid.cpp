//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/geometry/Ellipsoid.cpp
//! @brief     Implements class Ellipsoid
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "base/geometry/Ellipsoid.h"
#include "base/geometry/AABB.h"

#include <array>

namespace ohkl {

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, const Eigen::Matrix3d& metric)
    : _center(center), _metric(metric), _inverseMetric(metric.inverse()), _aabb()
{
    updateAABB();
}

Ellipsoid::Ellipsoid(
    const Eigen::Vector3d& center, const Eigen::Vector3d& radii, const Eigen::Matrix3d& axes)
{
    Eigen::Matrix3d D = Eigen::Matrix3d::Identity();
    for (auto i = 0; i < 3; ++i)
        D(i, i) = 1.0 / (radii[i] * radii[i]);

    // By definition, we have A.U = U.D where A is the metric tensor, U is the
    // matric of columned eigen-vectors and D the diagonal matrix of corresponding
    // eigen values
    _metric = axes * D * axes.transpose();
    _inverseMetric = _metric.inverse();
    _center = center;
    updateAABB();
}

Ellipsoid::Ellipsoid(const Eigen::Vector3d& center, double radius)
{
    _metric = Eigen::Matrix3d::Identity() / (radius * radius);
    _inverseMetric = _metric.inverse();
    _center = center;
    updateAABB();
}

bool Ellipsoid::collide(const AABB& aabb) const
{
    // trivial check: center of ellipsoid is inside AABB
    if (aabb.isInside(_center))
        return true;

    const std::vector<Eigen::Vector3d> normals = {
        Eigen::Vector3d(1, 0, 0),
        Eigen::Vector3d(0, 1, 0),
        Eigen::Vector3d(0, 0, 1),
    };

    const auto& lb = aabb.lower();
    const auto& ub = aabb.upper();
    const auto dx = ub - lb;

    // calculate vertices and check for collision with each of the 6 faces of the
    // BB
    for (int i = 0; i < 3; ++i) {
        auto n0 = normals[i];
        auto n1 = normals[(i + 1) % 3];
        auto n2 = normals[(i + 2) % 3];

        auto a = n0 * n0.dot(dx);
        auto b = n1 * n1.dot(dx);

        // check face touching lower bound of BB
        if (collideFace(lb, a, b, n2))
            return true;
        // check face touching upper bound of BB
        if (collideFace(ub, -a, -b, n2))
            return true;
    }
    return false;
}

// See Lemma 3 of "Continuous Collision Detection for Elliptic Disks"
// by Choi, Wang, and Liu. The lemma is stated for ellipses but one can
// easily check that the proof is valid for ellipsoids in all dimensions.
bool Ellipsoid::collide(const Ellipsoid& other) const
{
    // quick test using AABB, also needed for numerical stability
    if (!_aabb.collide(other._aabb))
        return false;

    // Gets roots of characteristic equation
    const auto& AI = homogeneousMatrixInverse();
    const auto& B = other.homogeneousMatrix();
    Eigen::ComplexEigenSolver<Eigen::Matrix4d> solver(AI * B);
    const auto& roots = solver.eigenvalues();

    const double eps = 1e-5;

    // if there exists a real negative root then the ellipsoids are separated
    for (auto i = 0; i < 4; ++i) {
        if (std::fabs(imag(roots(i))) < eps && real(roots(i)) < 0.0)
            return false;
    }
    return true;
}

void Ellipsoid::rotate(const Eigen::Matrix3d& U)
{
    _metric = U * _metric * U.transpose();
    updateAABB();
}

void Ellipsoid::scale(double value)
{
    _metric /= value * value;
    _inverseMetric *= value * value;
    updateAABB();
}

void Ellipsoid::translate(const Eigen::Vector3d& t)
{
    _center += t;
    updateAABB();
}

bool Ellipsoid::isInside(const Eigen::Vector3d& point) const
{
    Eigen::Vector3d u = point - _center;
    return u.dot(_metric * u) <= 1.0;
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
    static constexpr double c = 4.0 * M_PI / 3.0;
    return c * std::pow(_metric.determinant(), -0.5);
}

void Ellipsoid::updateAABB()
{
    Eigen::Vector3d a;
    for (auto i = 0; i < 3; ++i)
        a(i) = std::sqrt(_inverseMetric(i, i));
    Eigen::Vector3d lb(_center - a);
    Eigen::Vector3d ub(_center + a);
    _aabb = AABB(lb, ub);
}

const AABB& Ellipsoid::aabb() const
{
    return _aabb;
}

Eigen::Matrix4d Ellipsoid::homogeneousMatrix() const
{
    Eigen::Matrix4d Q = Eigen::Matrix4d::Zero();
    Q.block<3, 3>(0, 0) = _metric;
    Q.block<3, 1>(0, 3) = -_metric * _center;
    Q.block<1, 3>(3, 0) = (-_metric * _center).transpose();
    Q(3, 3) = _center.dot(_metric * _center) - 1.0;
    return Q;
}

Eigen::Matrix4d Ellipsoid::homogeneousMatrixInverse() const
{
    Eigen::Matrix4d Q = Eigen::Matrix4d::Zero();
    Q.block<3, 3>(0, 0) = _inverseMetric - _center * _center.transpose();
    Q.block<3, 1>(0, 3) = -_center;
    Q.block<1, 3>(3, 0) = -_center.transpose();
    Q(3, 3) = -1.0;
    return Q;
}

Eigen::Vector3d Ellipsoid::radii() const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_metric);
    Eigen::Vector3d vals = solver.eigenvalues();

    for (auto i = 0; i < 3; ++i)
        vals(i) = 1.0 / std::sqrt(vals(i));
    return vals;
}

const Eigen::Matrix3d& Ellipsoid::inverseMetric() const
{
    return _inverseMetric;
}

// this is a simple optimization problem: look for the minimum value
// of (x-x0).dot(A(x-x0)) along the segment from a to b. Either this occurs at
// one of the endpoints, or it occurs at a critical point of f(t) =
// (x(t)-x0).dot(A(x(t)-x0)) where x(t) = a + t(b-a).
bool Ellipsoid::collideSegment(const Eigen::Vector3d& a, const Eigen::Vector3d& b) const
{
    if (isInside(a))
        return true;
    if (isInside(b))
        return true;

    // endpoints do not intersect ellipsoid, so now we look for the critical point
    const Eigen::Vector3d ba = b - a;
    const Eigen::Vector3d Aba = _metric * ba;
    const double t = -(a - _center).dot(Aba) / ba.dot(Aba);

    // critical point occurs outside the segment
    if (t < 0 || t > 1)
        return false;
    // critical point is inside the segment; check whether it is in the ellipsoid
    return isInside(a + t * ba);
}

// this is a simple optimization problem: look for minimum value of
// (x-x0).dot(A(x-x0)) on the speficied face. First find the critical point on
// the plane, and check whether it is actually contained in the face. If not,
// then we have to check each of the segments bounding the face.
bool Ellipsoid::collideFace(
    const Eigen::Vector3d& o, const Eigen::Vector3d& a, const Eigen::Vector3d& b,
    const Eigen::Vector3d& n) const
{
    const double d = n.dot(o);
    const double nAn = n.dot(_inverseMetric * n);
    const double lagrange = (d - n.dot(_center)) / nAn;

    // ellipsoid does not even intersect the plane containing the face
    if (lagrange * lagrange * nAn > 1.0)
        return false;

    // x is the point where (x-x0).dot(A*(x-x0)) attains its minimum on the plane
    const Eigen::Vector3d x(_center + lagrange * _inverseMetric * n);

    // check that the point x is contained in the face
    const double t = a.dot(x - o) / a.dot(a);
    const double s = b.dot(x - o) / b.dot(b);

    // minimum is in the face
    if (t >= 0 && s >= 0 && t <= 1 && s <= 1)
        return true;

    // last possible case: minimum is attained on some boundary segment
    if (collideSegment(o, o + a))
        return true;
    if (collideSegment(o, o + b))
        return true;
    if (collideSegment(o + a, o + a + b))
        return true;
    if (collideSegment(o + b, o + a + b))
        return true;
    return false;
}

Eigen::Vector3d Ellipsoid::intersectionCenter(
    const Eigen::Vector3d& n, const Eigen::Vector3d& p) const
{
    const auto& AI = _inverseMetric;
    const auto AIn = AI * n;
    const double lambda = (p.dot(n) - _center.dot(n)) / n.dot(AIn);
    return _center + lambda * AIn;
}

double Ellipsoid::r2(const Eigen::Vector3d x) const
{
    return (x - _center).dot(_metric * (x - _center));
}

} // namespace ohkl
