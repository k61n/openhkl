#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class PValuePeakFilter : public IPeakFilter {

public:

    static IPeakFilter* create();

    PValuePeakFilter();

    PValuePeakFilter(const std::map<std::string,double>& parameters);

    virtual ~PValuePeakFilter()=default;

    virtual IPeakFilter* clone() const override;

    PValuePeakFilter& operator=(const PValuePeakFilter& other)=default;

    virtual bool valid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
