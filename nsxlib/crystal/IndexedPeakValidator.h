#pragma once

#include <map>
#include <string>

#include "CrystalTypes.h"
#include "PeakValidator.h"

namespace nsx {

class IndexedPeakValidator : public PeakValidator {

public:

    static PeakValidator* create(const std::map<std::string,double>& parameters);

    IndexedPeakValidator();

    IndexedPeakValidator(const std::map<std::string,double>& parameters);

    virtual ~IndexedPeakValidator()=default;

    IndexedPeakValidator& operator=(const IndexedPeakValidator& other)=default;

    bool isValid(sptrPeak3D peak) const override;

    virtual std::string description() const override;

    virtual void setParameters(const std::map<std::string,double>& parameters) override;
};

} // end namespace nsx
