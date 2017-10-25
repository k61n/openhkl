#include "FrameInterval.h"
#include "../crystal/Peak3D.h"
#include "../data/DataSet.h"

namespace nsx {

FrameInterval::FrameInterval(sptrDataSet data):
    _data(data),
    _fmin(0.0),
    _fmax(data->getNFrames()-1)
{
}

FrameInterval::FrameInterval(sptrDataSet data, double fmin, double fmax):
    _data(data),
    _fmin(fmin),
    _fmax(fmax)
{
}

#if 0
PeakSet FrameInterval::peaks() const
{
    PeakSet selected_peaks;
    for (auto&& p : _data->getPeaks()) {
        auto&& frame = p->getIntegrationRegion().getRegion().center()[2];
        if (frame >= _fmin && frame <= _fmax) {
            selected_peaks.insert(p);
        }
    }
    return selected_peaks;
}
#endif

} // end namespace nsx
