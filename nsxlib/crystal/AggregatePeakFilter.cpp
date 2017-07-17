#include <algorithm>

#include "AggregatePeakFilter.h"
#include "IPeakFilter.h"

namespace nsx {

AggregatePeakFilter::AggregatePeakFilter() : _filters()
{
}

AggregatePeakFilter::AggregatePeakFilter(const AggregatePeakFilter& other)
{
    for (auto filter : other._filters)
    {
        _filters.emplace_back(filter->clone());
    }
}

AggregatePeakFilter::~AggregatePeakFilter()
{
    for (auto filter : _filters) {
        delete filter;
    }
}

IPeakFilter* AggregatePeakFilter::clone() const
{
    return (new AggregatePeakFilter(*this));
}

bool AggregatePeakFilter::valid(sptrPeak3D peak) const
{
    for (auto&& filter : _filters) {
        if (!filter->valid(peak)) {
            return false;
        }
    }
    return true;
}

void AggregatePeakFilter::addFilter(const IPeakFilter& filter) {

    auto it=std::find(_filters.begin(),_filters.end(),&filter);
    if (it == _filters.end()) {
        _filters.emplace_back(filter.clone());
    }
}

} // end namespace nsx
