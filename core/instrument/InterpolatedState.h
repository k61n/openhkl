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

#pragma once

#include "DirectVector.h"
#include "InstrumentState.h"
#include "ReciprocalVector.h"

#include <Eigen/Core>

namespace nsx {

//! Class to store an interpolation between two instrument states.
//! In addition to the data fields stored in an InstrumentState, this class
//! also stores information about the (angular) velocity and step size, which
//! is needed to compute analytic derivatives of various functions.
class InterpolatedState : public InstrumentState {
public:
    //! Default value needed for SWIG (note: nullptr does _not_ work)
    InterpolatedState(Diffractometer* diffractometer = nullptr);
    //! Construct by interpolation. The paramter t should be between 0 and 1.
    InterpolatedState(const InstrumentState& s1, const InstrumentState& s2, double t);
    //! Compute the jacobian of the transformation (x,y,frame) -> q_sample
    Eigen::Matrix3d jacobianQ(double px, double py) const;
    //! Compute the Lorentz factor at the detector coordinates (px, py).
    double lorentzFactor(double px, double py) const;
    //! The axis of crystal rotation, in _sample space_.
    Eigen::Vector3d axis;
    //! Step size between the two underlying InstrumentStates, in radians
    double stepSize;

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif
};

} // end namespace nsx
