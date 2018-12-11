
#include "PeakData.h"
#include "DataSet.h"
#include "Detector.h"
#include "DetectorEvent.h"
#include "Diffractometer.h"
#include "DirectVector.h"
#include "InstrumentState.h"
#include "Peak3D.h"
#include "ReciprocalVector.h"

namespace nsx {

PeakData::PeakData(sptrPeak3D peak) : _peak(peak), _system(peak), _events(), _counts(), _coords() {}

const std::deque<DetectorEvent>& PeakData::events() const
{
    return _events;
}

const std::deque<double>& PeakData::counts() const
{
    return _counts;
}

void PeakData::computeStandard()
{
    if (_peak == nullptr) {
        throw std::runtime_error(
            "PeakData::computeStandard() cannot be called if _peak is nullptr");
    }

    _coords.resize(_events.size());

    for (size_t i = 0; i < _events.size(); ++i) {
        _coords[i] = _system.transform(_events[i]);
    }
}

void PeakData::addEvent(const DetectorEvent& ev, double count)
{
    _events.push_back(ev);
    _counts.push_back(count);
}

void PeakData::reset()
{
    std::deque<DetectorEvent> e;
    std::deque<double> c;
    std::deque<Eigen::Vector3d> crds;

    std::swap(_events, e);
    std::swap(_counts, c);
    std::swap(_coords, crds);
}

} // end namespace nsx
