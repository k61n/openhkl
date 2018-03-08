#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Peak3D.h"
#include "PeakCoordinateSystem.h"

namespace nsx {

PeakCoordinateSystem::PeakCoordinateSystem(sptrPeak3D peak): 
    _peak([peak]() {
        if (!peak) {
            throw std::runtime_error("PeakCoordinateSystem: cannot constuct with null Peak3D");
        }
        return peak;
    }()),

    _event(peak->getShape().center()),

    _state(peak->data()->interpolatedState(_event._frame)),

    _ki(_state.ki().rowVector()),

    _kf([&]() {
        // take care to make sure that q is transformed to lab coordinate system
        // question: better to use observed q or predicted q??
        auto detector = peak->data()->diffractometer()->getDetector();
        auto pos = detector->pixelPosition(_event._px, _event._py);
        return _state.kfLab(pos).rowVector();
    }()),
    
    _e1(_kf.cross(_ki)),
    _e2(_kf.cross(_e1))
{
    _e1.normalize();
    _e2.normalize();

    _zeta = _e1.dot(_state.axis) * 180.0 / M_PI * _state.stepSize;
    _e1 *= 180.0 / M_PI / _kf.norm();
    _e2 *= 180.0 / M_PI / _kf.norm();
}

Eigen::Vector3d PeakCoordinateSystem::transform(const DetectorEvent& ev) const
{
    auto det = _peak->data()->diffractometer()->getDetector();
    auto position = det->pixelPosition(ev._px, ev._py);
    const Eigen::RowVector3d dk = _state.kfLab(position).rowVector() - _kf;

    const double eps1 = _e1.dot(dk);
    const double eps2 = _e2.dot(dk);
    const double eps3 = _zeta * (ev._frame - _event._frame);

    return Eigen::Vector3d(eps1, eps2, eps3);
}

Eigen::Matrix3d PeakCoordinateSystem::jacobian() const
{
    Eigen::Matrix3d dkdx = _state.jacobianK(_event);

    // Jacobian of epsilon coordinates
    Eigen::Matrix3d J;

    J.row(0) = _e1.transpose() * dkdx;
    J.row(1) = _e2.transpose() * dkdx;
    J.row(2) = Eigen::RowVector3d(0, 0, _zeta);

    return J;    
}

Ellipsoid PeakCoordinateSystem::detectorShape(double sigmaD, double sigmaM) const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Vector3d center = _peak->getShape().center();

    Eigen::Matrix3d metric;
    metric.setZero();

    // inverse covariance in standard coordinates
    metric(0,0) = metric(1,1) = 1 / sigmaD / sigmaD;
    metric(2,2) = 1 / sigmaM / sigmaM;

    // transform inverse covariance to detector space
    Eigen::Matrix3d detector_metric = J.transpose() * metric * J;

    return Ellipsoid(center, detector_metric);
}

Ellipsoid PeakCoordinateSystem::standardShape() const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Matrix3d cov = J * _peak->getShape().inverseMetric() * J.transpose();
    return Ellipsoid(_peak->q().rowVector(), cov.inverse());
}

double PeakCoordinateSystem::estimateDivergence() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(0.5 * (C(0,0) + C(1,1)));
}

double PeakCoordinateSystem::estimateMosaicity() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(C(2,2));
}

} // end namespace nsx
