#include "Peak3D.h"
#include "PValuePeakFilter.h"

namespace nsx {

IPeakFilter* PValuePeakFilter::create()
{
    return new PValuePeakFilter();
}

IPeakFilter* PValuePeakFilter::clone() const
{
    return (new PValuePeakFilter(*this));
}

PValuePeakFilter::PValuePeakFilter() : IPeakFilter()
{
    _parameters["p-value"] = 0.8;
}

PValuePeakFilter::PValuePeakFilter(const std::map<std::string,double>& parameters) : IPeakFilter(parameters)
{
}

bool PValuePeakFilter::valid(sptrPeak3D peak) const
{
    if (!_activated) {
        return true;
    }

    return peak->pValue() < _parameters.at("p-value");
}

std::string PValuePeakFilter::description() const
{
    return "Filter peaks statistically not distinguishable from background";
}

void PValuePeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
    auto it = parameters.find("p-value");
    if (it != parameters.end()) {
        _parameters["p-value"] = it->second;
    }
}

} // end namespace nsx
