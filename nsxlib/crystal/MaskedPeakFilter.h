#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class MaskedPeakFilter : public IPeakFilter {

public:

    static IPeakFilter* create();

    MaskedPeakFilter();

    MaskedPeakFilter(const std::map<std::string,double>& parameters);

    virtual ~MaskedPeakFilter()=default;

    virtual IPeakFilter* clone() const override;

    MaskedPeakFilter& operator=(const MaskedPeakFilter& other)=default;

    virtual bool valid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
