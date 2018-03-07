#pragma once

#include <deque>
#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "GeometryTypes.h"
#include "InstrumentTypes.h"
#include "ReciprocalVector.h"
#include "PeakCoordinateSystem.h"

namespace nsx {

class PeakData {
public:
    PeakData(sptrPeak3D peak = nullptr);

    PeakData(PeakData&& other) = default;

    const std::deque<DetectorEvent>& events() const;
    const std::deque<double>& counts() const;
    const std::deque<ReciprocalVector>& qs() const;

    //! Compute the q values for each of the observed events.
    void computeQs();

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
    std::deque<ReciprocalVector> _qs;
    std::deque<Eigen::Vector3d> _coords;
};

} // end namespace nsx
