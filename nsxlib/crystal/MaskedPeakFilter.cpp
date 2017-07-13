#include "MaskedPeakFilter.h"
#include "Peak3D.h"

namespace nsx {

IPeakFilter* MaskedPeakFilter::create(const std::map<std::string,double>& parameters)
{
    return new MaskedPeakFilter(parameters);
}

MaskedPeakFilter::MaskedPeakFilter() : IPeakFilter()
{
}

MaskedPeakFilter::MaskedPeakFilter(const std::map<std::string,double>& parameters) : IPeakFilter(parameters)
{
    _parameters.clear();
}

bool MaskedPeakFilter::valid(sptrPeak3D peak) const
{
    return !(peak->isMasked());
}

std::string MaskedPeakFilter::description() const
{
    return "Filter masked peaks";
}

void MaskedPeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
}

} // end namespace nsx
