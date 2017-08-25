#ifndef NSXLIB_CRYSTALTYPES_H
#define NSXLIB_CRYSTALTYPES_H

#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace nsx {

class MergedPeak;
class Peak3D;
class PeakCalc;
class PeakIntegrator;
class PeakRecord;
class Profile;
class SpaceGroup;
class SpaceGroupSymbols;
class SymOp;
class UnitCell;

using UnitCellSolution = std::pair<UnitCell,double>;

using sptrPeak3D   = std::shared_ptr<Peak3D>;
using sptrUnitCell = std::shared_ptr<UnitCell>;

using PeakSet    = std::set<sptrPeak3D>;
using MergedPeakSet = std::set<MergedPeak>;

using PeakList     = std::vector<sptrPeak3D>;
using PeakCalcList = std::vector<PeakCalc>;
using RecordList   = std::vector<PeakRecord>;
using SymOpList    = std::vector<SymOp>;
using UnitCellList = std::vector<sptrUnitCell>;

using SpaceGroupSymmetry = std::pair<std::string,std::string>;

using AutoIndexingSoluce = std::pair<UnitCell,double>;

} // end namespace nsx

#endif // NSXLIB_CRYSTALTYPES_H
