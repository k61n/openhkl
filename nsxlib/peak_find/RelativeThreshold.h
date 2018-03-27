#pragma once

#include "Threshold.h"

namespace nsx {

class RelativeThreshold : public Threshold {

public:

    RelativeThreshold();

    RelativeThreshold(const std::map<std::string,double>& parameters);

    ~RelativeThreshold()=default;

    double value(sptrDataSet dataset, int frame) const override;

    const char* name() const;
};

} // end namespace nsx
