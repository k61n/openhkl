//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/Blob3D.cpp
//! @brief     Implements class Blob3D
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <limits>
#include <stdexcept>

#include <Eigen/Eigenvalues>

#include "core/integration/Blob3D.h"

static const double minimum_blob_mass = 1e-15;

namespace ohkl {

Blob3D::Blob3D()
    : _m0(0)
    , _m1(Eigen::Vector3d::Zero())
    , _m2(Eigen::Matrix3d::Zero())
    , _npoints(0)
    , _minValue(std::numeric_limits<double>::max())
    , _maxValue(std::numeric_limits<double>::min())
    , _valid(true)
{
}

Blob3D::Blob3D(double x, double y, double z, double m)
{
    Eigen::Vector3d v(x, y, z);
    _m0 = m;
    _m1 = m * v;
    _m2 = m * v * v.transpose();
    _npoints = 1;
    _minValue = m;
    _maxValue = m;
    _valid = true;
}

void Blob3D::addPoint(double x, double y, double z, double m)
{
    Eigen::Vector3d v(x, y, z);
    _m0 += m;
    _m1 += m * v;
    _m2 += m * v * v.transpose();
    ++_npoints;

    if (m < _minValue)
        _minValue = m;
    else if (m > _maxValue)
        _maxValue = m;
}

void Blob3D::merge(const Blob3D& b)
{
    _m0 += b._m0;
    _m1 += b._m1;
    _m2 += b._m2;

    _npoints += b._npoints;
    _minValue = (_minValue < b._minValue ? _minValue : b._minValue);
    _maxValue = (_maxValue > b._maxValue ? _maxValue : b._maxValue);
}

double Blob3D::getMass() const
{
    return _m0;
}
int Blob3D::getComponents() const
{
    return _npoints;
}

double Blob3D::getMinimumMass() const
{
    return _minValue;
}

double Blob3D::getMaximumMass() const
{
    return _maxValue;
}

Eigen::Vector3d Blob3D::center() const
{
    if (_m0 < minimum_blob_mass)
        _valid = false;
    return _m1 / _m0;
}

void Blob3D::printSelf(std::ostream& os) const
{
    os << "#Blob center: " << center().transpose() << std::endl;
    os << "Mass: " << _m0 << std::endl;
    os << "Points in the blob: " << _npoints << std::endl;
}

// todo: remove non-const reference args, just return Ellipsoid3D
bool Blob3D::toEllipsoid(
    double scale, Eigen::Vector3d& c, Eigen::Vector3d& eigenvalues,
    Eigen::Matrix3d& eigenvectors) const
{
    if (_m0 < minimum_blob_mass) {
        _valid = false;
        return false;
    }

    // Center of the ellipsoid
    c = center();

    // Define the variance-covariance/inertia tensor (inverse of the metric
    // tensor)
    Eigen::Matrix3d inertia = _m2 / _m0 - c * c.transpose();
    inertia /= scale * scale;

    // todo(jonathan): rewrite so that we no longer use eigenvalue solver (new
    // Ellipsoid implementation)
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver;
    solver.compute(inertia);

    // This is the Gaussian sigma along three directions
    // (fabs is a safe-guard against very small negative eigenvalues due to
    // precision errors)
    eigenvalues << sqrt(std::fabs(solver.eigenvalues()[0])),
        sqrt(std::fabs(solver.eigenvalues()[1])), sqrt(std::fabs(solver.eigenvalues()[2]));

    // Now eigenvectors
    eigenvectors = solver.eigenvectors();
    return true;
}

std::ostream& operator<<(std::ostream& os, const Blob3D& b)
{
    b.printSelf(os);
    return os;
}

Eigen::Matrix3d Blob3D::covariance() const
{
    Eigen::Vector3d com = _m1 / _m0;
    Eigen::Matrix3d cov = _m2 / _m0 - com * com.transpose();
    return cov;
}

bool Blob3D::isValid() const
{
    return _valid;
}

} // namespace ohkl
