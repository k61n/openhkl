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

InstrumentState InstrumentState::interpolate(const InstrumentState &other, double t) const
{
    InstrumentState result(*this);

    result.detector = detector.interpolate(other.detector, t);
    result.sample = sample.interpolate(other.sample, t);
    result.source = source.interpolate(other.source, t);

    const double s = 1-t;

    result.detectorOrientation = interpolateRotation(detectorOrientation, other.detectorOrientation, t);
    result.detectorOffset = s*detectorOffset + t*other.detectorOffset;

    result.sampleOrientation = interpolateRotation(sampleOrientation, other.sampleOrientation, t);
    result.samplePosition = s*samplePosition + t*other.samplePosition;

    result.ni = s*ni + t*other.ni;
    result.wavelength = s*wavelength + t*other.wavelength;

    return result;
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
    auto ki = ni.normalized() / wavelength;
    auto qLab = kfLab(detector_position).rowVector() - ki;
    return ReciprocalVector(qLab*sampleOrientation);
}

void InstrumentState::getGammaNu(double& gamma, double& nu, const DirectVector& detector_position) const
{
    auto kf = kfLab(detector_position).rowVector();
    gamma = std::atan2(kf[0], kf[1]);
    nu = std::asin(kf[2] / kf.norm());
}

double InstrumentState::getLorentzFactor(const DirectVector& detector_position) const
{
    double gamma,nu;
    getGammaNu(gamma, nu, detector_position);
    double lorentz = 1.0/(sin(std::fabs(gamma))*cos(nu));
    return lorentz;
}

double InstrumentState::get2Theta(const DirectVector& detector_position) const
{
    auto kf = kfLab(detector_position).rowVector();  
    double proj = kf.dot(ni);
    return acos(proj/kf.norm()/ni.norm());
}

ReciprocalVector InstrumentState::ki() const
{
    return ReciprocalVector(ni/ni.norm()/wavelength);
}

} // end namespace nsx
