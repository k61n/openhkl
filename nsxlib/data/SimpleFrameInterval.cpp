#include "SimpleFrameInterval.h"

#include <sstream>
#include <stdexcept>

#include "../crystal/Peak3D.h"
#include "../data/DataSet.h"

namespace nsx {

SimpleFrameInterval::SimpleFrameInterval(DataSet& data) : IFrameInterval(data)
{
    _index_min = 0;
    _index_max = data.getNFrames()-1;
}

SimpleFrameInterval::SimpleFrameInterval(DataSet& data, int index_min, int index_max) : IFrameInterval(data)
{
    if (index_min < 0 || index_max > data.getNFrames()-1) {
        std::stringstream ss;
        ss << "invalid index: must be in [0,"<<data.getNFrames()<<"[";
        throw std::runtime_error(ss.str());
    }

    _index_min = 0;
    _index_max = data.getNFrames()-1;
}

PeakSet SimpleFrameInterval::peaks() const
{
    PeakSet selected_peaks;
    for (auto&& p : _data.getPeaks()) {
        auto&& frame = p->getIntegrationRegion().getRegion().center()[2];
        if (frame < _index_min || frame > _index_max) {
            selected_peaks.insert(p);
        }
    }
    return selected_peaks;
}

} // end namespace nsx
