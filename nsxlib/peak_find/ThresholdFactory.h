#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "DataTypes.h"
#include "PeakFindTypes.h"

namespace nsx {

class ThresholdFactory {

public:

    using callback = std::function<sptrThreshold(const std::map<std::string,double>&)>;

    ThresholdFactory();

    sptrThreshold create(const std::string& threshold_type, const std::map<std::string,double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~ThresholdFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
