#pragma once

#include <deque>
#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "GeometryTypes.h"
#include "InstrumentTypes.h"
#include "ReciprocalVector.h"

namespace nsx {

class PeakData {
public:
    PeakData(sptrPeak3D peak = nullptr);

    PeakData(PeakData&& other) = default;

    const std::deque<DetectorEvent>& events() const;
    const std::deque<double>& counts() const;
    const std::deque<ReciprocalVector> qs() const;

    //! Compute the q values for each of the observed events.
    void computeQs();

    //! Add an event to the list of events.
    void addEvent(const DetectorEvent& ev, double count);

    //! Clear the events
    void reset();

private:
    sptrPeak3D _peak;
    std::deque<DetectorEvent> _events;
    std::deque<double> _counts;
    std::deque<ReciprocalVector> _qs;
};

} // end namespace nsx
