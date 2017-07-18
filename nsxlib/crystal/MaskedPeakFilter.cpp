#include "MaskedPeakFilter.h"
#include "Peak3D.h"

namespace nsx {

IPeakFilter* MaskedPeakFilter::create()
{
    return new MaskedPeakFilter();
}

IPeakFilter* MaskedPeakFilter::clone() const
{
    return (new MaskedPeakFilter(*this));
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
    if (!_activated) {
        return true;
    }

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
