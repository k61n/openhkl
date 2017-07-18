#pragma once

#include <vector>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class AggregatePeakFilter : public IPeakFilter {

public:

    AggregatePeakFilter();

    virtual ~AggregatePeakFilter();

    AggregatePeakFilter(const AggregatePeakFilter& other);

    AggregatePeakFilter& operator=(const AggregatePeakFilter& other)=delete;

    virtual IPeakFilter* clone() const override;

    void addFilter(const IPeakFilter& filter);

    std::vector<IPeakFilter*> filters();

    bool valid(sptrPeak3D peak) const override;

private:

    std::vector<IPeakFilter*> _filters;

};

} // end namespace nsx
