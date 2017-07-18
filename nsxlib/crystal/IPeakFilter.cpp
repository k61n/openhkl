#include "IPeakFilter.h"

namespace nsx {

IPeakFilter::IPeakFilter() : _parameters(), _activated(true)
{
}

IPeakFilter::IPeakFilter(const std::map<std::string,double>& parameters) : _parameters(parameters), _activated(true)
{
}

const std::map<std::string,double>& IPeakFilter::parameters() const
{
    return _parameters;
}

void IPeakFilter::setParameters(const std::map<std::string,double>& parameters)
{
}

std::string IPeakFilter::description() const
{
    return "";
}

PeakSet IPeakFilter::filter(const PeakSet& peaks) const
{
    PeakSet filtered_peaks;

    for (auto&& peak : peaks)
    {
        if (valid(peak)) {
            filtered_peaks.insert(peak);
        }
    }

    return filtered_peaks;
}

bool IPeakFilter::activated() const
{
    return _activated;
}

void IPeakFilter::setActivated(bool activated)
{
    _activated = activated;
}

} // end namespace nsx
