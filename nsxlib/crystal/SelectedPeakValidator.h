#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class SelectedPeakValidator : public PeakValidator {

public:

    SelectedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~SelectedPeakValidator()=default;

    SelectedPeakValidator& operator=(const SelectedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
