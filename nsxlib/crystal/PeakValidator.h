#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"

namespace nsx {

class PeakValidator {

public:

    PeakValidator(const std::map<std::string,double>& parameters);

    PeakValidator(const PeakValidator& other)=default;

    virtual ~PeakValidator()=default;

    PeakValidator& operator=(const PeakValidator& other);

    virtual bool isValid(sptrPeak3D peak) const=0;

protected:

    std::map<std::string,double> _parameters;

};

} // end namespace nsx

