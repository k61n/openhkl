#include "IPeakFilter.h"

namespace nsx {

IPeakFilter::IPeakFilter() : _parameters()
{
}

IPeakFilter::IPeakFilter(const std::map<std::string,double>& parameters) : _parameters(parameters)
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

} // end namespace nsx
