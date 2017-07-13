#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "IPeakFilter.h"

namespace nsx {

class SelectedPeakFilter : public IPeakFilter {

public:

    static IPeakFilter* create(const std::map<std::string,double>& parameters);

    SelectedPeakFilter();

    SelectedPeakFilter(const std::map<std::string,double>& parameters);

    virtual ~SelectedPeakFilter()=default;

    SelectedPeakFilter& operator=(const SelectedPeakFilter& other)=default;

    virtual bool valid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
