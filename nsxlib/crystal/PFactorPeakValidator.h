#pragma once

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class PFactorPeakValidator : public PeakValidator {

public:

    using PeakValidator::PeakValidator;

    virtual ~PFactorPeakValidator()=default;

    PFactorPeakValidator& operator=(const PFactorPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
