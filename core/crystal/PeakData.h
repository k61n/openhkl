#ifndef CORE_CRYSTAL_PEAKDATA_H
#define CORE_CRYSTAL_PEAKDATA_H

#include <Eigen/Dense>
#include <deque>

#include "DetectorEvent.h"
#include "PeakCoordinateSystem.h"

namespace nsx {

//! \class PeakData
//! \brief Helper class used by integration routines.
class PeakData {
public:
    //! Construct instance associated to the given peak
    PeakData(sptrPeak3D peak = nullptr);
    //! Return the list of detector events associated to the peak
    const std::deque<DetectorEvent>& events() const;
    //! Return the list of detector counts associated to the peak
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

} // end namespace nsx

#endif // CORE_CRYSTAL_PEAKDATA_H
