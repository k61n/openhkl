//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/InterpolatedState.h
//! @brief     Defines class InterpolatedState
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INSTRUMENT_INTERPOLATEDSTATE_H
#define NSX_CORE_INSTRUMENT_INTERPOLATEDSTATE_H

#include "core/instrument/InstrumentState.h"

namespace nsx {

//! Stores an interpolation between two instrument states.

//! In addition to the data fields stored in an InstrumentState, this class
//! also stores information about the (angular) velocity and step size, which
//! is needed to compute analytic derivatives of various functions.

class InterpolatedState : public InstrumentState {
 public:
    //! Default value needed for SWIG (note: nullptr does _not_ work)
    InterpolatedState(Diffractometer* diffractometer = nullptr);
    //! Construct by interpolation. The paramter t should be between 0 and 1.
    InterpolatedState(
        const InstrumentState& s1, const InstrumentState& s2, double t, bool valid = true);
    //! Compute the jacobian of the transformation (x,y,frame) -> q_sample
    Eigen::Matrix3d jacobianQ(double px, double py) const;
    //! Compute the Lorentz factor at the detector coordinates (px, py).
    double lorentzFactor(double px, double py) const;
    //! The axis of crystal rotation, in _sample space_.
    Eigen::Vector3d axis;
    //! Step size between the two underlying InstrumentStates, in radians
    double stepSize;

    //! Interpolates the state for a given frame index (not necessarily integer)
    static InterpolatedState interpolate(const InstrumentStateList& states,
                                         const double frame_idx);

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif
};

} // namespace nsx

#endif // NSX_CORE_INSTRUMENT_INTERPOLATEDSTATE_H
