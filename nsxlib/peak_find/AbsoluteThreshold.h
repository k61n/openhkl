#pragma once

#include "Threshold.h"

namespace nsx {

class AbsoluteThreshold : public Threshold {

public:

    AbsoluteThreshold();

    AbsoluteThreshold(const std::map<std::string,double>& parameters);

    ~AbsoluteThreshold()=default;

    double value(sptrDataSet dataset, int frame) const override;

    const char* name() const;
};

} // end namespace nsx
