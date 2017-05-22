#ifndef NSXLIB_CRYSTALTYPES_H
#define NSXLIB_CRYSTALTYPES_H

#include <memory>
#include <vector>

namespace nsx {

class MergedPeak;
class Peak3D;
class UnitCell;

using sptrPeak3D = std::shared_ptr<Peak3D>;
using sptrUnitCell = std::shared_ptr<UnitCell>;

using PeakList = std::vector<sptrPeak3D>;

} // end namespace nsx

#endif // NSXLIB_CRYSTALTYPES_H
