#ifndef NSXLIB_CRYSTALTYPES_H
#define NSXLIB_CRYSTALTYPES_H

#include <memory>
#include <set>
#include <vector>

namespace nsx {

class MergedPeak;
class Peak3D;
class PeakRecord;
class SpaceGroup;
class SymOp;
class UnitCell;

using sptrPeak3D   = std::shared_ptr<Peak3D>;
using sptrUnitCell = std::shared_ptr<UnitCell>;

using PeakSet    = std::set<sptrPeak3D>;

using PeakList     = std::vector<sptrPeak3D>;
using RecordList   = std::vector<PeakRecord>;
using SymOpList    = std::vector<SymOp>;
using UnitCellList = std::vector<sptrUnitCell>;

} // end namespace nsx

#endif // NSXLIB_CRYSTALTYPES_H
