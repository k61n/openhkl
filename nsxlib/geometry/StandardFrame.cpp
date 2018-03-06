#include "DataSet.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Peak3D.h"
#include "StandardFrame.h"

namespace nsx {

StandardFrame::StandardFrame(sptrPeak3D peak)
{
    if (!peak) {
        throw std::runtime_error("StandardFrame: cannot constuct with null Peak3D");
    }

    _peak = peak;
    _frame = peak->getShape().center()[2];
    _state = peak->data()->interpolatedState(_frame);

    _ki = _state.ki().rowVector();
    // take care to make sure that q is transformed to lab coordinate system
    // question: better to use observed q or predicted q??
    _kf = _ki + peak->q().rowVector() * _state.sampleOrientation().transpose();

    _e1 = _kf.cross(_ki);
    _e2 = _kf.cross(_e1);

    _e1.normalize();
    _e2.normalize();
  
    Eigen::Vector3d axis = _state.axis;
    _zeta = _e1.dot(axis) * 180.0 / M_PI * _state.stepSize;
    _e1 *= 180.0 / M_PI / _kf.norm();
    _e2 *= 180.0 / M_PI / _kf.norm();
}

Eigen::Vector3d StandardFrame::transform(const DetectorEvent& ev) const
{
    auto det = _peak->data()->diffractometer()->getDetector();
    auto position = det->pixelPosition(ev._px, ev._py);
    const Eigen::RowVector3d dk = _state.kfLab(position).rowVector() - _kf;

    const double eps1 = _e1.dot(dk);
    const double eps2 = _e2.dot(dk);
    const double eps3 = _zeta * (ev._frame - _frame);

    return Eigen::Vector3d(eps1, eps2, eps3);
}

Eigen::Matrix3d StandardFrame::jacobian() const
{
    const Eigen::Vector3d center = _peak->getShape().center();
    DetectorEvent ev(center[0], center[1], center[2]);
    auto detector = _peak->data()->diffractometer()->getDetector();

    // Jacobian from (px, py, frame) to lab coordinates on detector
    Eigen::Matrix3d dpdx = detector->jacobian(center[0], center[1]);

    const double ki = _state.ki().rowVector().norm();

    // postion in lab space on the detector
    Eigen::Vector3d p = detector->pixelPosition(center[0], center[1]).vector();

    // Jacobian of position -> kf
    Eigen::Vector3d dp = p - _state.samplePosition;
    double r = dp.norm();

    Eigen::RowVector3d drdx = 0.5 * dp.transpose() * dpdx;

    // Jacobian of (px, py) -> kf
    Eigen::Matrix3d dkdx = ki * (dpdx / r - dp * drdx / r / r);

    // Jacobian of epsilon coordinates
    Eigen::Matrix3d J;

    J.row(0) = _e1.transpose() * dkdx;
    J.row(1) = _e2.transpose() * dkdx;
    J.row(2) = Eigen::RowVector3d(0, 0, _zeta);

    return J;    
}

Ellipsoid StandardFrame::detectorShape(double sigmaD, double sigmaM) const
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

Ellipsoid StandardFrame::standardShape() const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Matrix3d cov = J * _peak->getShape().inverseMetric() * J.transpose();
    return Ellipsoid(_peak->q().rowVector(), cov.inverse());
}

double StandardFrame::estimateDivergence() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(0.5 * (C(0,0) + C(1,1)));
}

double StandardFrame::estimateMosaicity() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return std::sqrt(C(2,2));
}

} // end namespace nsx
