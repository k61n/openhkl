#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class PFactorPeakValidator : public PeakValidator {

public:

    PFactorPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~PFactorPeakValidator()=default;

    PFactorPeakValidator& operator=(const PFactorPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
