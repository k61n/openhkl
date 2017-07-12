#pragma once

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class IndexedPeakValidator : public PeakValidator {

public:

    using PeakValidator::PeakValidator;

    virtual ~IndexedPeakValidator()=default;

    IndexedPeakValidator& operator=(const IndexedPeakValidator& other)=default;

    bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
