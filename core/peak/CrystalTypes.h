#pragma once

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class MergedPeak;
class Peak3D;
class PeakIntegrator;
class PeakValidator;
class Profile;
class UnitCell;

using UnitCellSolution = std::pair<UnitCell,double>;

using sptrPeak3D   = std::shared_ptr<Peak3D>;
using sptrUnitCell = std::shared_ptr<UnitCell>;

using RankedSolution = std::pair<sptrUnitCell,double>;

using PeakList = std::vector<sptrPeak3D>;

using MergedPeakSet = std::set<MergedPeak>;

using UnitCellList = std::vector<sptrUnitCell>;

} // end namespace nsx
