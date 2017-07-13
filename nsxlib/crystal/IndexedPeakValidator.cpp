#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../data/DataSet.h"
#include "IndexedPeakValidator.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

IndexedPeakValidator::IndexedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
}

bool IndexedPeakValidator::isValid(sptrPeak3D peak) const
{
    nsx::UnitCell cell = *(peak->getActiveUnitCell());
    Eigen::RowVector3d hkl;
    return peak->getMillerIndices(cell,hkl,true);
}

} // end namespace nsx
