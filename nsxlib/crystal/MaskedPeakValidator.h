#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class MaskedPeakValidator : public PeakValidator {

public:

    MaskedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~MaskedPeakValidator()=default;

    MaskedPeakValidator& operator=(const MaskedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
