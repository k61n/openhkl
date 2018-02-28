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

    _e1 *= 180.0 / M_PI / _kf.norm();
    _e2 *= 180.0 / M_PI / _kf.norm();

    Eigen::Vector3d axis = _state.axis;
    _zeta = _e1.dot(axis) * 180.0 / M_PI * _state.stepSize;
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


} // end namespace nsx
