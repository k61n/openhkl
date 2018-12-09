#pragma once

#include <memory>
#include <vector>
#include "DataSet.h"
#include "SpaceGroup.h"

namespace nsx {

class Peak3D;
using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;

//! Mask a given peak
void maskPeaks(const DataSet *const dataSet, PeakList& peaks);

//! Friedel pairs (if this is not already a symmetry of the space group)
std::vector<PeakList> findEquivalences(
    const SpaceGroup& spaceGroup, const PeakList &peak_list, bool friedel);


} // end namespace nsx
