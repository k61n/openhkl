//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InstrumentState.cpp
//! @brief     Implements class InstrumentState
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InstrumentState.h"

#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"

#include <Eigen/Dense>
#include <algorithm>
#include <cassert>

namespace nsx {

InstrumentState::InstrumentState(Diffractometer* diffractometer) : _diffractometer(diffractometer)
{
    detectorOrientation.setIdentity();

    sampleOrientation = Eigen::Quaterniond(1, 0, 0, 0);

    sampleOrientationOffset = Eigen::Quaterniond(1, 0, 0, 0);

    samplePosition.setZero();

    detectorPositionOffset.setZero();

    ni = {0.0, 1.0, 0.0};

    wavelength = 1.0;

    refined = false;
}

ReciprocalVector InstrumentState::kfLab(const DirectVector& detector_position) const
{
    Eigen::Vector3d k = detectorOrientation * (detector_position.vector() - samplePosition);
    k.normalize();
    k /= wavelength;
    return ReciprocalVector(k);
}

void InstrumentState::adjustKi(const DirectVector& detector_position)
{
    ni = detectorOrientation * (detector_position.vector() - samplePosition);
    ni.normalize();
}

ReciprocalVector InstrumentState::sampleQ(const DirectVector& detector_position) const
{
    Eigen::RowVector3d qLab(kfLab(detector_position).rowVector() - ki().rowVector());
    return ReciprocalVector(qLab * sampleOrientationMatrix());
}

double InstrumentState::gamma(const DirectVector& detector_position) const
{
    Eigen::RowVector3d kf = kfLab(detector_position).rowVector();
    return std::atan2(kf[0], kf[1]);
}

double InstrumentState::nu(const DirectVector& detector_position) const
{
    Eigen::RowVector3d kf = kfLab(detector_position).rowVector();
    return std::asin(kf[2] / kf.norm());
}

double InstrumentState::twoTheta(const DirectVector& detector_position) const
{
    auto kf = kfLab(detector_position).rowVector();
    double proj = kf.dot(ni);
    return acos(proj / kf.norm() / ni.norm());
}

ReciprocalVector InstrumentState::ki() const
{
    return ReciprocalVector((ni / ni.norm()) / wavelength);
}

Eigen::Matrix3d InstrumentState::detectorOrientationMatrix() const
{
    return detectorOrientation;
}

Eigen::Matrix3d InstrumentState::sampleOrientationMatrix() const
{
    return (sampleOrientationOffset * sampleOrientation).normalized().toRotationMatrix();
}

Eigen::Matrix3d InstrumentState::jacobianK(double px, double py) const
{
    const auto* detector = _diffractometer->detector();

    // Jacobian from (px, py, frame) to lab coordinates on detector
    Eigen::Matrix3d dpdx = detector->jacobian(px, py);

    const double nki = ki().rowVector().norm();

    // postion in lab space on the detector
    Eigen::Vector3d p = detector->pixelPosition(px, py).vector();

    // Jacobian of position -> kf
    Eigen::Vector3d dp = p - samplePosition;
    double r = dp.norm();

    Eigen::RowVector3d drdx = 1 / r * dp.transpose() * dpdx;

    // Jacobian of (px, py) -> kf
    Eigen::Matrix3d dkdx = nki * detectorOrientation * (dpdx / r - dp * drdx / r / r);

    return dkdx;
}

const Diffractometer* InstrumentState::diffractometer() const
{
    return _diffractometer;
}

Diffractometer* InstrumentState::diffractometer()
{
    return _diffractometer;
}

InstrumentState InstrumentState::state(
    Diffractometer* const diffractometer, const std::size_t frame_idx)
{
    assert(frame_idx < diffractometer->sampleStates.size());
    assert(frame_idx < diffractometer->detectorStates.size());

    // compute transformations
    const auto& detector_gonio = diffractometer->detector()->gonio();
    const auto& sample_gonio = diffractometer->sample().gonio();

    Eigen::Transform<double, 3, Eigen::Affine> detector_trans =
        detector_gonio.affineMatrix(diffractometer->detectorStates[frame_idx]);
    Eigen::Transform<double, 3, Eigen::Affine> sample_trans =
        sample_gonio.affineMatrix(diffractometer->sampleStates[frame_idx]);

    InstrumentState state_(const_cast<Diffractometer*>(diffractometer));
    state_.detectorOrientation = detector_trans.rotation();
    state_.sampleOrientation = Eigen::Quaterniond(sample_trans.rotation());

    state_.detectorPositionOffset = detector_trans.translation();
    state_.samplePosition = sample_trans.translation();

    state_.ni = diffractometer->source().selectedMonochromator().ki().rowVector();
    state_.ni.normalize();
    state_.wavelength = diffractometer->source().selectedMonochromator().wavelength();

    return state_;
}

bool InstrumentState::isValid() const
{
    return _valid;
}

} // namespace nsx
