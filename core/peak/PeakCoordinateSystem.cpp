//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/PeakCoordinateSystem.cpp
//! @brief     Implements class PeakCoordinateSystem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/peak/PeakCoordinateSystem.h"
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/peak/Peak3D.h"

namespace nsx {

//    if (!_peak)
//        throw std::runtime_error("Cannot construct PeakCoordinateSystem from null Peak3d");

PeakCoordinateSystem::PeakCoordinateSystem(Peak3D* peak) : _peak(peak)
{
    _event = DetectorEvent(peak->shape().center());
    _state = peak->dataSet()->instrumentStates().interpolate(_event._frame);
    _ki = _state.ki().rowVector();
    _kf = peak->q().rowVector() * _state.sampleOrientationMatrix().transpose() + _ki;

    _e1 = _kf.cross(_ki);
    _e2 = _kf.cross(_e1);

    _e1.normalize();
    _e2.normalize();

    _zeta = _e1.dot(_state.axis) * 180.0 / M_PI * _state.stepSize;
    _e1 *= 180.0 / M_PI / _kf.norm();
    _e2 *= 180.0 / M_PI / _kf.norm();
}

Eigen::Vector3d PeakCoordinateSystem::transform(const DetectorEvent& ev) const
{
    auto position = _peak->dataSet()->detector().pixelPosition(ev._px, ev._py);
    const Eigen::RowVector3d dk = _state.kfLab(position).rowVector() - _kf;

// Kabsch coordinate system
#if 1
    const double eps1 = _e1.dot(dk);
    const double eps2 = _e2.dot(dk);
    const double eps3 = _zeta * (ev._frame - _event._frame);
#else

    // new coordinate system?
    const Eigen::RowVector3d dq = _state.axis.cross(_kf - _ki) * _state.stepSize;
    const Eigen::RowVector3d dk2 = dk + (ev._frame - _event._frame) * dq;
    const double eps1 = _e1.dot(dk2);
    const double eps2 = _e2.dot(dk2);
    const double eps3 = _kf.dot(dq) / _kf.norm() / (_kf - _ki).norm() * (ev._frame - _event._frame);
#endif

    return Eigen::Vector3d(eps1, eps2, eps3);
}

Eigen::Matrix3d PeakCoordinateSystem::jacobian() const
{
    Eigen::Matrix3d dkdx = _state.jacobianK(_event._px, _event._py);

    // Jacobian of epsilon coordinates
    Eigen::Matrix3d J;

// Kabsch coordinate system
#if 1
    J.row(0) = _e1.transpose() * dkdx;
    J.row(1) = _e2.transpose() * dkdx;
    J.row(2) = Eigen::RowVector3d(0, 0, _zeta);
#else

    // new coordinate system?
    J.setZero();

    const Eigen::RowVector3d dq = _state.axis.cross(_kf - _ki) * _state.stepSize;

    J.row(0) = _e1.transpose() * dkdx;
    J(0, 2) += _e1.dot(dq);

    J.row(1) = _e2.transpose() * dkdx;
    J(1, 2) += _e2.dot(dq);

    J(2, 2) = _kf.dot(dq) / _kf.norm() / (_kf - _ki).norm();

#endif

    return J;
}

Ellipsoid PeakCoordinateSystem::detectorShape(double sigmaD, double sigmaM) const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Vector3d center = _peak->shape().center();

    Eigen::Matrix3d metric;
    metric.setZero();

    // inverse covariance in standard coordinates
    metric(0, 0) = metric(1, 1) = 1 / sigmaD / sigmaD;
    metric(2, 2) = 1 / sigmaM / sigmaM;

    // transform inverse covariance to detector space
    Eigen::Matrix3d detector_metric = J.transpose() * metric * J;

    return Ellipsoid(center, detector_metric);
}

Ellipsoid PeakCoordinateSystem::standardShape() const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Matrix3d cov = J * _peak->shape().inverseMetric() * J.transpose();
    return Ellipsoid(_peak->q().rowVector(), cov.inverse());
}

double PeakCoordinateSystem::estimateDivergence() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(0.5 * (C(0, 0) + C(1, 1)));
}

double PeakCoordinateSystem::estimateMosaicity() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(C(2, 2));
}

} // namespace nsx
