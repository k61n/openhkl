#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class MaskedPeakValidator : public PeakValidator {

public:

    static PeakValidator* create(const std::map<std::string,double>& parameters);

    MaskedPeakValidator();

    MaskedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~MaskedPeakValidator()=default;

    MaskedPeakValidator& operator=(const MaskedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
