#include <algorithm>

#include "AggregatePeakFilter.h"
#include "IPeakFilter.h"

namespace nsx {

AggregatePeakFilter::AggregatePeakFilter() : _filters()
{
}

AggregatePeakFilter::~AggregatePeakFilter()
{
    for (auto filter : _filters) {
        delete filter;
    }
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

void AggregatePeakFilter::addFilter(IPeakFilter* filter) {

    auto it=std::find(_filters.begin(),_filters.end(),filter);
    if (it == _filters.end()) {
        _filters.push_back(filter);
    }
}

} // end namespace nsx
