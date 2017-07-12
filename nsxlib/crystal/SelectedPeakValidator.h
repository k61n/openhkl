#pragma once

namespace nsx {

class SelectedPeakValidator : public PeakValidator {

public:

    using PeakValidator::PeakValidator;

    virtual ~SelectedPeakValidator()=default;

    SelectedPeakValidator& operator=(const SelectedPeakValidator& other)=default;

    virtual bool isValid(sptrPeak3D peak) const override;
};

} // end namespace nsx
