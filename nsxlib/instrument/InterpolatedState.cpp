/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent Chapon, Eric Pelligrini, Jonathan Fisher

 chapon[at]ill.fr
 pellegrini[at]ill.fr
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "Component.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "InterpolatedState.h"
#include "Logger.h"
#include "MatrixOperations.h"

namespace nsx {

InterpolatedState::InterpolatedState(sptrDiffractometer diffractometer): InstrumentState(diffractometer)
{
    
}

InterpolatedState::InterpolatedState(const InstrumentState& s1, const InstrumentState& s2, double t): 
    InstrumentState(s1.diffractometer()),
    transformation(),
    axis(),
    stepSize()
{
    if (s1.diffractometer() != s2.diffractometer()) {
        throw std::runtime_error("Cannot interpolate states between different diffractometers");
    }

    const double s = 1-t;

    detectorOrientation = interpolateRotation(s1.detectorOrientation, s2.detectorOrientation, t);
    detectorOffset = s*s1.detectorOffset + t*s2.detectorOffset;

    Eigen::Quaterniond q1 = s1.sampleOrientationOffset*s1.sampleOrientation;
    Eigen::Quaterniond q2 = s2.sampleOrientationOffset*s2.sampleOrientation;

    q1.normalize();
    q2.normalize();

    sampleOrientation = q1.slerp(t, q2);
    sampleOrientationOffset = Eigen::Quaterniond(1, 0, 0, 0);

    samplePosition = s*s1.samplePosition + t*s2.samplePosition;

    ni = s*(s1.ni / s1.ni.norm()) + t*(s2.ni / s2.ni.norm());
    wavelength = s*s1.wavelength + t*s2.wavelength;

    //Eigen::Quaterniond q = q2 * q1.inverse();
    Eigen::Quaterniond q = q1.inverse() * q2;
    axis = q.vec().normalized();
    q.normalize();
    const double cos_theta2 = q.w();
    const double sin_theta2 = q.vec().norm();
    stepSize = 2.0*std::atan2(sin_theta2, cos_theta2);
}

Eigen::Matrix3d InterpolatedState::jacobianQ(double px, double py) const
{
    auto position = _diffractometer->getDetector()->pixelPosition(px, py);
    Eigen::Vector3d q0 = sampleQ(position).rowVector();
    // Jacobian of map from detector coords to sample q space
    Eigen::Matrix3d J = sampleOrientationMatrix().transpose() * jacobianK(px, py); 
    // take into account the rotation
    // negative sign is due to treating q as a column vector instead of a row vector
    J.col(2) = -stepSize * axis.cross(q0);
    return J;
}

double InterpolatedState::lorentzFactor(double px, double py) const
{
    auto position = _diffractometer->getDetector()->pixelPosition(px, py);
    Eigen::Vector3d q0 = sampleQ(position).rowVector();
    Eigen::Vector3d kf = sampleOrientationMatrix().transpose() * kfLab(position).rowVector().transpose();
    const double lorentz = kf.norm() / std::fabs(kf.dot(axis.cross(q0)));
    return lorentz;
}

} // end namespace nsx
