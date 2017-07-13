#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../data/DataSet.h"

#include "IndexedPeakFilter.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

IPeakFilter* IndexedPeakFilter::create(const std::map<std::string,double>& parameters)
{
    return new IndexedPeakFilter(parameters);
}

IndexedPeakFilter::IndexedPeakFilter() : IPeakFilter()
{
}

IndexedPeakFilter::IndexedPeakFilter(const std::map<std::string,double>& parameters) : IPeakFilter(parameters)
{
    _parameters.clear();
}

bool IndexedPeakFilter::valid(sptrPeak3D peak) const
{
    nsx::UnitCell cell = *(peak->getActiveUnitCell());
    Eigen::RowVector3d hkl;
    return peak->getMillerIndices(cell,hkl,true);
}

std::string IndexedPeakFilter::description() const
{
    return "Filter unindexed peaks";
}

void IndexedPeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
