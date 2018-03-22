#include "AbsoluteThreshold.h"
#include "RelativeThreshold.h"
#include "ThresholdFactory.h"

namespace nsx {

template <typename T>
sptrThreshold create_threshold(const std::map<std::string,double>& parameters)
{
    return std::make_shared<T>(parameters);
}

ThresholdFactory::ThresholdFactory(): _callbacks()
{
    _callbacks["absolute"] = &create_threshold<AbsoluteThreshold>;
    _callbacks["relative"] = &create_threshold<RelativeThreshold>;
}

sptrThreshold ThresholdFactory::create(const std::string& threshold_type, const std::map<std::string,double>& parameters) const
{
    const auto it = _callbacks.find(threshold_type);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error(threshold_type + " is not registered as a valid threshold");
    }

    return (it->second)(parameters);
}

const std::map<std::string,ThresholdFactory::callback>& ThresholdFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx

