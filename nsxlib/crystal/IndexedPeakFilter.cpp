#include "../instrument/Diffractometer.h"
#include "../instrument/Sample.h"
#include "../data/DataSet.h"

#include "IndexedPeakFilter.h"
#include "Peak3D.h"
#include "UnitCell.h"

namespace nsx {

IPeakFilter* IndexedPeakFilter::create()
{
    return new IndexedPeakFilter();
}

IPeakFilter* IndexedPeakFilter::clone() const
{
    return (new IndexedPeakFilter(*this));
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
    if (!_activated) {
        return true;
    }

    auto cell = peak->getActiveUnitCell();
    if (!cell) {
        return true;
    }

    Eigen::RowVector3d hkl;
    return peak->getMillerIndices(*cell,hkl,true);
}

std::string IndexedPeakFilter::description() const
{
    return "Filter unindexed peaks";
}

void IndexedPeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
