//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InterpolatedState.cpp
//! @brief     Implements class InterpolatedState
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/InterpolatedState.h"
#include "core/detector/Detector.h"
#include "core/gonio/Component.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/MatrixOperations.h"
#include <stdexcept>
#include <string> // to_string


namespace nsx {

InterpolatedState::InterpolatedState(Diffractometer* diffractometer)
    : InstrumentState(diffractometer)
{
    _valid = false;
}

InterpolatedState::InterpolatedState(
    const InstrumentState& s1, const InstrumentState& s2, double t, bool valid)
    : InstrumentState(const_cast<Diffractometer*>(s1.diffractometer())), axis(), stepSize()
{
    _valid = valid;
    if (s1.diffractometer() != s2.diffractometer())
        throw std::runtime_error("Cannot interpolate states between different diffractometers");

    const double s = 1 - t;

    detectorOrientation = interpolateRotation(s1.detectorOrientation, s2.detectorOrientation, t);
    detectorPositionOffset = s * s1.detectorPositionOffset + t * s2.detectorPositionOffset;

    Eigen::Quaterniond q1 = s1.sampleOrientationOffset * s1.sampleOrientation;
    Eigen::Quaterniond q2 = s2.sampleOrientationOffset * s2.sampleOrientation;

    q1.normalize();
    q2.normalize();

    sampleOrientation = q1.slerp(t, q2);
    sampleOrientationOffset = Eigen::Quaterniond(1, 0, 0, 0);

    samplePosition = s * s1.samplePosition + t * s2.samplePosition;

    ni = s * (s1.ni / s1.ni.norm()) + t * (s2.ni / s2.ni.norm());
    wavelength = s * s1.wavelength + t * s2.wavelength;

    // Eigen::Quaterniond q = q2 * q1.inverse();
    Eigen::Quaterniond q = q1.inverse() * q2;
    axis = q.vec().normalized();
    q.normalize();
    const double cos_theta2 = q.w();
    const double sin_theta2 = q.vec().norm();
    stepSize = 2.0 * std::atan2(sin_theta2, cos_theta2);
}

InterpolatedState InterpolatedState::interpolate(
    const InstrumentStateList& states, const double frame_idx)
{
    bool valid = true;
    const std::size_t states_nr = states.size();
    if (frame_idx > (states_nr - 2) || frame_idx < 0)
        valid = false;

    const std::size_t idx = std::size_t(std::lround(std::floor(frame_idx)));
    const std::size_t next = std::min(idx + 1, states_nr - 1);

    if (idx == next) // I *think* this only happens on the last frame of the data set - zamaan
        valid = false;

    return InterpolatedState(states.at(idx), states.at(next), frame_idx - idx, valid);
}

Eigen::Matrix3d InterpolatedState::jacobianQ(double px, double py) const
{
    auto position = _diffractometer->detector()->pixelPosition(px, py);
    Eigen::Vector3d q0 = sampleQ(position).rowVector();
    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = sampleOrientationMatrix().transpose() * jacobianK(px, py);
    // take into account the rotation
    // negative sign is due to treating q as a column vector instead of a row
    // vector
    J.col(2) = -stepSize * axis.cross(q0);
    return J;
}

double InterpolatedState::lorentzFactor(double px, double py) const
{
    auto position = _diffractometer->detector()->pixelPosition(px, py);
    Eigen::Vector3d q0 = sampleQ(position).rowVector();
    Eigen::Vector3d kf =
        sampleOrientationMatrix().transpose() * kfLab(position).rowVector().transpose();
    const double lorentz = kf.norm() / std::fabs(kf.dot(axis.cross(q0)));
    return lorentz;
}

} // namespace nsx
