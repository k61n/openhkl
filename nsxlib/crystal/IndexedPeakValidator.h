#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class IndexedPeakValidator : public PeakValidator {

public:

    IndexedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~IndexedPeakValidator()=default;

    IndexedPeakValidator& operator=(const IndexedPeakValidator& other)=default;

    bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
