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
#include "InstrumentTypes.h"
#include "ReciprocalVector.h"

#include <Eigen/Core>

namespace nsx {

//! Class storing the state of the experiment at a given moment of time.
//! State refers to any parameters which might change during the experiment:
//! sample orientation, sample position, etc. States are initially loaded
//! as metadata but can also be refined as part of the data treatment.
class InstrumentState {
public:
    //! default value needed for SWIG (note: nullptr does _not_ work)
    InstrumentState(sptrDiffractometer diffractomer = sptrDiffractometer());
    //! Destructor
    virtual ~InstrumentState() {}
    //! Detector orientation as read from metadata
    Eigen::Matrix3d detectorOrientation;
    //! compute the sample orientation from fixed orientation and offset
    Eigen::Matrix3d sampleOrientationMatrix() const;
    //! Sample orientation as read from metadata
    Eigen::Quaterniond sampleOrientation;
    //! Offset to sample orientation, used for parameter refinement
    Eigen::Quaterniond sampleOrientationOffset;
    //! Sample position
    Eigen::Vector3d samplePosition;
    //! Detector offset.
    Eigen::Vector3d detectorPositionOffset;
    //! Incoming beam direction.
    Eigen::RowVector3d ni;
    //! Incoming beam wavelength.
    double wavelength;
    //! Takes a direct vector in detector coordinates and computes kf in lab coordinates
    ReciprocalVector kfLab(const DirectVector& detector_position) const;
    //! Return source ki
    ReciprocalVector ki() const;
    //! Takes direct vector in detector coordinates and computes q in sample coordinates
    ReciprocalVector sampleQ(const DirectVector& detector_position) const;
    //! Return the gamma angle associated to the given lab space position
    double gamma(const DirectVector& detector_position) const;
    //! Return the nu angle associated to the given lab space position
    double nu(const DirectVector& detector_position) const;
    //! Return the 2*theta angle associated to the given lab space position
    double twoTheta(const DirectVector& detector_position) const;
    //! Compute the jacobian of the transformation (x,y) -> k_lab
    Eigen::Matrix3d jacobianK(double px, double py) const;
    //! Return the diffractometer of the state
    sptrDiffractometer diffractometer() const;

    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

protected:
    //! Pointer to the diffractometer whose state this object stores
    sptrDiffractometer _diffractometer;
};

} // end namespace nsx
