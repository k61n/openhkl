//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/peak/Qs2Events.h
//! @brief     Defines function qs2events
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_PEAK_QS2EVENTS_H
#define NSX_CORE_PEAK_QS2EVENTS_H

#include "base/utils/ProgressHandler.h"
#include "core/detector/Detector.h"
#include "core/instrument/InstrumentStateList.h"

namespace nsx {

namespace algo {

/*! \brief Determine the sample rotation(s) at which the Ewald sphere is intersected
 *
 * This algorithm determines the sample rotation angle (i.e. non-integer frame number) at which
 * the Ewald sphere is intersected by a q-vector. Given a list of q-vectors corresponding to Miller
 * indices, it will return a list of detector events (i.e. peak centres), determining the correct
 * angle (frame) for each.
 *
 * For each q-vector, we determine whether the sign of |k_f| - |k_i| is different between the first
 * and last frames. If it is, we use a bisection search to determine the angle (frame value) at
 * at which |k_f| = |k_i|, and use this value to get an interpolated instrument state, and then
 * A detector event (peak centre).
 *
 * Zamaan 2/6/2021 - in the original version, it was assumed that there is *one* root in the entire
 * frame range, which is incorrect. I have fixed this by splitting the range into n_interval equal
 * intervals and then performing the bisection algorithm over each interval.
 */

//! Returns detector events corresponding to the list of q values.
std::vector<DetectorEvent> qs2events(
    const std::vector<ReciprocalVector>& sample_qs, const InstrumentStateList& states,
    const Detector& detector, const int n_intervals, sptrProgressHandler handler = nullptr);

} // namespace algo

} // namespace nsx

#endif // NSX_CORE_PEAK_QS2EVENTS_H
