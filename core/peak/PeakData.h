//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/quantify/PeakData.h
//! @brief     Defines class PeakData
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_PEAK_PEAKDATA_H
#define CORE_PEAK_PEAKDATA_H

#include <Eigen/Dense>
#include <deque>

#include "core/peak/PeakCoordinateSystem.h"

namespace nsx {

//! \class PeakData
//! Helper class used by integration routines.
class PeakData {
public:
    //! Construct instance associated to the given peak
    PeakData(sptrPeak3D peak = nullptr);
    //! Returns the list of detector events associated to the peak
    const std::deque<DetectorEvent>& events() const;
    //! Returns the list of detector counts associated to the peak
    const std::deque<double>& counts() const;
    //! Compute the standard coordinates for each of the observed events.
    void computeStandard();
    //! Add an event to the list of events.
    void addEvent(const DetectorEvent& ev, double count);
    //! Clear the events
    void reset();

private:
    sptrPeak3D _peak;
    PeakCoordinateSystem _system;
    std::deque<DetectorEvent> _events;
    std::deque<double> _counts;
    std::deque<Eigen::Vector3d> _coords;
};

} // namespace nsx

#endif // CORE_PEAK_PEAKDATA_H
