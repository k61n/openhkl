#include "FrameInterval.h"
#include "../crystal/Peak3D.h"
#include "../data/DataSet.h"

namespace nsx {

SimpleFrameInterval::SimpleFrameInterval(sptrDataSet data):
    _data(data),
    _index_min(0),
    _index_max(data->getNFrames()-1)
{
}

SimpleFrameInterval::SimpleFrameInterval(sptrDataSet data, size_t index_min, size_t index_max):
    _data(data),
    _index_min(std::max<size_t>(index_min,0)),
    _index_max(std::min<size_t>(index_max,data->getNFrames()-1))
{
}

PeakSet SimpleFrameInterval::peaks() const
{
    PeakSet selected_peaks;
    for (auto&& p : _data->getPeaks()) {
        auto&& frame = p->getIntegrationRegion().getRegion().center()[2];
        if (frame >= _index_min && frame <= _index_max) {
            selected_peaks.insert(p);
        }
    }
    return selected_peaks;
}

} // end namespace nsx
