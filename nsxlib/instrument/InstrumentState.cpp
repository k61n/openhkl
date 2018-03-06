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
#include "InstrumentState.h"
#include "MatrixOperations.h"

namespace nsx {

InstrumentState::InstrumentState()
{
    detectorOrientation.setIdentity();
    sampleOrientation = Eigen::Quaterniond(1, 0, 0, 0);
    sampleOrientationOffset = Eigen::Quaterniond(1, 0, 0, 0);
    samplePosition.setZero();
    detectorOffset.setZero();
    ni = {0.0, 1.0, 0.0};
    wavelength = 1.0;
}

ReciprocalVector InstrumentState::kfLab(const DirectVector& detector_position) const
{
    Eigen::Vector3d k = detectorOrientation*(detector_position.vector() - samplePosition);
    k.normalize();
    k /= wavelength;
    return ReciprocalVector(k);
}

ReciprocalVector InstrumentState::sampleQ(const DirectVector& detector_position) const
{
    Eigen::RowVector3d ki = ni / ni.norm() / wavelength;
    auto qLab = kfLab(detector_position).rowVector() - ki;
    return ReciprocalVector(qLab*sampleOrientationMatrix());
}

double InstrumentState::gamma(const DirectVector& detector_position) const
{
    Eigen::RowVector3d kf = kfLab(detector_position).rowVector();
    double gamma = std::atan2(kf[0], kf[1]);
    return gamma;
}

double InstrumentState::nu(const DirectVector& detector_position) const
{
    Eigen::RowVector3d kf = kfLab(detector_position).rowVector();
    double nu = std::asin(kf[2] / kf.norm());
    return nu;
}

double InstrumentState::lorentzFactor(const DirectVector& detector_position) const
{
    double g = gamma(detector_position);
    double n = nu(detector_position);
    double lorentz = 1.0/(sin(std::fabs(g))*cos(n));
    return lorentz;
}

double InstrumentState::twoTheta(const DirectVector& detector_position) const
{
    auto kf = kfLab(detector_position).rowVector();  
    double proj = kf.dot(ni);
    return acos(proj/kf.norm()/ni.norm());
}

ReciprocalVector InstrumentState::ki() const
{
    return ReciprocalVector(ni/ni.norm()/wavelength);
}

Eigen::Matrix3d InstrumentState::sampleOrientationMatrix() const
{
    return (sampleOrientationOffset*sampleOrientation).normalized().toRotationMatrix();
}



} // end namespace nsx
