#pragma once

#include <vector>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class AggregatePeakFilter : public IPeakFilter {

public:

    AggregatePeakFilter();

    virtual ~AggregatePeakFilter();

    AggregatePeakFilter(const AggregatePeakFilter& other)=delete;

    AggregatePeakFilter& operator=(const AggregatePeakFilter& other)=delete;

    void addFilter(IPeakFilter* validator);

    bool valid(sptrPeak3D peak) const override;

private:

    std::vector<IPeakFilter*> _filters;

};

} // end namespace nsx
