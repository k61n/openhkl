#pragma once

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

    const std::vector<DetectorEvent>& events() const;
    const std::vector<double>& counts() const;
    const std::vector<ReciprocalVector> qs() const;

    //! Compute the q values for each of the observed events.
    void computeQs();

    //! Add an event to the list of events.
    void addEvent(const DetectorEvent& ev, double count);

    //! Clear the events
    void reset();

private:
    sptrPeak3D _peak;
    std::vector<DetectorEvent> _events;
    std::vector<double> _counts;
    std::vector<ReciprocalVector> _qs;
};

} // end namespace nsx
