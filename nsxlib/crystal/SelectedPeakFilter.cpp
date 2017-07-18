#include "Peak3D.h"
#include "SelectedPeakFilter.h"

namespace nsx {

IPeakFilter* SelectedPeakFilter::create()
{
    return new SelectedPeakFilter();
}

IPeakFilter* SelectedPeakFilter::clone() const
{
    return (new SelectedPeakFilter(*this));
}

SelectedPeakFilter::SelectedPeakFilter() : IPeakFilter()
{
}

SelectedPeakFilter::SelectedPeakFilter(const std::map<std::string,double>& parameters) : IPeakFilter(parameters)
{
    _parameters.clear();
}

bool SelectedPeakFilter::valid(sptrPeak3D peak) const
{
    if (!_activated) {
        return true;
    }

    return (peak->isSelected());
}

std::string SelectedPeakFilter::description() const
{
    return "Filter unselected peaks";
}

void SelectedPeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
