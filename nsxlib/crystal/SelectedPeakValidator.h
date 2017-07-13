#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class SelectedPeakValidator : public PeakValidator {

public:

    static PeakValidator* create(const std::map<std::string,double>& parameters);

    SelectedPeakValidator();

    SelectedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~SelectedPeakValidator()=default;

    SelectedPeakValidator& operator=(const SelectedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
