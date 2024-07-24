//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/PeakData.h
//! @brief     Defines class PeakData
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_PEAK_PEAKDATA_H
#define OHKL_CORE_PEAK_PEAKDATA_H

#include "core/peak/PeakCoordinateSystem.h"

#include <deque>

namespace ohkl {

//! Helper class used by integration routines.

class PeakData {
 public:
    //! Construct instance associated to the given peak
    PeakData(Peak3D* peak = nullptr);
    //! Returns the list of detector events associated with the peak
    const std::deque<DetectorEvent>& events() const;
    //! Returns the list of detector counts associated with the peak
    const std::deque<double>& counts() const;
    //! Returns the list of gradients associated with the peak
    const std::deque<double>& gradients() const;
    //! Compute the standard coordinates for each of the observed events.
    void standardizeCoords();
    //! Add an event to the list of events.
    void addEvent(const DetectorEvent& ev, double count, double gradient = 0);
    //! Append a PeakData object to this one
    void append(const PeakData& other);
    //! Clear the events
    void reset();
    //! Is this PeakData empty?:
    bool empty() const;
    //! Get the peak pointer
    Peak3D* peak() const { return _peak; };

 private:
    Peak3D* _peak;
    PeakCoordinateSystem _system;
    std::deque<DetectorEvent> _events;
    std::deque<double> _counts;
    std::deque<double> _gradients;
    std::deque<Eigen::Vector3d> _coords;
};

} // namespace ohkl

#endif // OHKL_CORE_PEAK_PEAKDATA_H
