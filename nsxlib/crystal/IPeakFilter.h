#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"

namespace nsx {

class IPeakFilter {

public:

    IPeakFilter();

    IPeakFilter(const std::map<std::string,double>& parameters);

    IPeakFilter(const IPeakFilter& other)=default;

    virtual ~IPeakFilter()=default;

    IPeakFilter& operator=(const IPeakFilter& other);

    virtual bool valid(sptrPeak3D peak) const=0;

    const std::map<std::string,double>& parameters() const;

    virtual void setParameters(const std::map<std::string,double>& parameters);

    virtual std::string description() const;

    PeakSet filter(const PeakSet& peaks) const;

protected:

    std::map<std::string,double> _parameters;

};

} // end namespace nsx

