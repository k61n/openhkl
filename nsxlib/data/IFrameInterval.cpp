#include "IFrameInterval.h"

#include "../data/DataSet.h"

namespace nsx {

IFrameInterval::IFrameInterval(DataSet& data) : _data(data) {
}

IFrameInterval::~IFrameInterval()=default;

} // end namespace nsx
