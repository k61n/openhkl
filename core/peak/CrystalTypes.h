#pragma once

#include <memory>
#include <vector>
#include <Eigen/Dense>

namespace nsx {

class Peak3D;
using sptrPeak3D   = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;

class UnitCell;
using sptrUnitCell = std::shared_ptr<UnitCell>;
using RankedSolution = std::pair<sptrUnitCell,double>;
using UnitCellList = std::vector<sptrUnitCell>;

} // end namespace nsx
