#pragma once

#include <memory>
#include <vector>
#include "DataSet.h"

namespace nsx {

class Peak3D;
using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;

//! Mask a given peak
void maskPeaks(const DataSet *const dataSet, PeakList& peaks);

} // end namespace nsx
