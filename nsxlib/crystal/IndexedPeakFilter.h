#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class IndexedPeakFilter : public IPeakFilter {

public:

    static IPeakFilter* create(const std::map<std::string,double>& parameters);

    IndexedPeakFilter();

    IndexedPeakFilter(const std::map<std::string,double>& parameters);

    virtual ~IndexedPeakFilter()=default;

    IndexedPeakFilter& operator=(const IndexedPeakFilter& other)=default;

    virtual bool valid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
