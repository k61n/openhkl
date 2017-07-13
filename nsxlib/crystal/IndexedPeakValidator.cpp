#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../data/DataSet.h"
#include "IndexedPeakValidator.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

PeakValidator* IndexedPeakValidator::create(const std::map<std::string,double>& parameters)
{
    return new IndexedPeakValidator(parameters);
}

IndexedPeakValidator::IndexedPeakValidator() : PeakValidator()
{
}

IndexedPeakValidator::IndexedPeakValidator(const std::map<std::string,double>& parameters) : PeakValidator(parameters)
{
    _parameters.clear();
}

bool IndexedPeakValidator::isValid(sptrPeak3D peak) const
{
    nsx::UnitCell cell = *(peak->getActiveUnitCell());
    Eigen::RowVector3d hkl;
    return peak->getMillerIndices(cell,hkl,true);
}

std::string IndexedPeakValidator::description() const
{
    return "Filter unindexed peaks";
}

void IndexedPeakValidator::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
