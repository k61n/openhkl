#pragma once

#include <vector>

#include "CrystalTypes.h"

namespace nsx {

class PeakFilter {

public:

    PeakFilter();

    virtual ~PeakFilter();

    PeakFilter(const PeakFilter& other)=delete;

    PeakFilter& operator=(const PeakFilter& other)=delete;

    void addValidator(PeakValidator* validator);

    PeakSet filter(const PeakSet& peaks) const;

private:

    std::vector<PeakValidator*> _validators;

};

} // end namespace nsx
