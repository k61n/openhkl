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

#include "core/detector/Detector.h"
#include "core/instrument/InstrumentStateList.h"

namespace nsx {

namespace algo {

//! Returns detector events corresponding to the list of q values.
std::vector<DetectorEvent> qs2events(
    const std::vector<ReciprocalVector>& sample_qs, const InstrumentStateList& states,
    const Detector& detector);

} // namespace algo

} // namespace nsx

#endif // NSX_CORE_PEAK_QS2EVENTS_H
