#pragma once

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class MaskedPeakValidator : public PeakValidator {

public:

    using PeakValidator::PeakValidator;

    virtual ~MaskedPeakValidator()=default;

    MaskedPeakValidator& operator=(const MaskedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
