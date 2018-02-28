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
    const Eigen::Vector3d p = _peak->getShape().center();
    DetectorEvent ev(p[0], p[1], p[2]);
    auto detector = _peak->data()->diffractometer()->getDetector();

    // Jacobian from (px, py, frame) to lab coordinates on detector
    Eigen::Matrix3d J_p = detector->jacobian(p[0], p[1]);

    // postion in lab space on the detector
    Eigen::Vector3d pos = detector->pixelPosition(p[0], p[1]).vector();

    // Jacobian of position -> kf
    Eigen::Vector3d dp = pos - _state.samplePosition;
    Eigen::Matrix3d J_kf;
    double r = dp.norm();
    J_kf.setIdentity();
    J_kf -= 1.0/(r*r) * dp * dp.transpose();
    J_kf /= r;

    // Jacobian of (px, py, frame) -> kf
    J_kf *= J_p;

    // finally, compute total Jacobian
    Eigen::Matrix3d J;
    J.setZero();
    J(0,0) = _e1.dot(J_kf.col(0));
    J(0,1) = _e1.dot(J_kf.col(1));
    J(1,0) = _e2.dot(J_kf.col(0));
    J(1,1) = _e2.dot(J_kf.col(1));
    J(0,2) = _zeta;

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

double StandardFrame::estimateDivergence() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return 0.5 * (C(0,0) + C(1,1));
}

double StandardFrame::estimateMosaicity() const
{
    auto shape = standardShape();
    auto C = shape.inverseMetric();
    return C(2,2);
}

Ellipsoid StandardFrame::standardShape() const
{
    Eigen::Matrix3d J = jacobian();
    Eigen::Matrix3d cov = J * _peak->getShape().inverseMetric() * J.transpose();
    return Ellipsoid(_peak->q().rowVector(), cov.inverse());
}

} // end namespace nsx
