
#include "DataSet.h"
#include "Detector.h"
#include "DetectorEvent.h"
#include "Diffractometer.h"
#include "DirectVector.h"
#include "InstrumentState.h"
#include "Peak3D.h"
#include "PeakData.h"
#include "ReciprocalVector.h"

namespace nsx {

PeakData::PeakData(sptrPeak3D peak): 
    _peak(peak),
    _events(), 
    _counts(), 
    _qs()
{

}

const std::vector<DetectorEvent>& PeakData::events() const
{
    return _events;
}

const std::vector<double>& PeakData::counts() const
{
    return _counts;
}
    
const std::vector<ReciprocalVector> PeakData::qs() const
{
    return _qs;
}

void PeakData::computeQs()
{
    if (_peak == nullptr) {
        throw std::runtime_error("PeakData::computeQs() cannot be called if _peak is nullptr");
    }

    _qs.resize(_events.size());
    auto data = _peak->data();
    auto detector = data->diffractometer()->getDetector();

    for (size_t i = 0; i < _events.size(); ++i) {
        auto state = data->interpolatedState(_events[i]._frame);        
        auto detector_position = DirectVector(detector->pixelPosition(_events[i]._px, _events[i]._py));
        _qs[i] = state.sampleQ(detector_position);
    }
}

void PeakData::addEvent(const DetectorEvent& ev, double count)
{
    _events.push_back(ev);
    _counts.push_back(count);
}

void PeakData::reset()
{
    _events.clear();
    _counts.clear();
    _qs.clear();

    _events.shrink_to_fit();
    _counts.shrink_to_fit();
    _qs.shrink_to_fit();
}

} // end namespace nsx
