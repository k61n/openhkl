#include <stdexcept>

#include "AbsoluteThreshold.h"

namespace nsx {

AbsoluteThreshold::AbsoluteThreshold()
: Threshold()
{
    _parameters["intensity"] = 3.0;
}

AbsoluteThreshold::AbsoluteThreshold(const std::map<std::string,double>& parameters)
: AbsoluteThreshold()
{
    auto it = parameters.find("intensity");
    if (it != parameters.end()) {
        _parameters["intensity"] = it->second;
    }
}

double AbsoluteThreshold::value(sptrDataSet dataset, int frame) const
{
    return _parameters.at("intensity");
}

const char* AbsoluteThreshold::name() const
{
    return "absolute";
}

} // end namespace nsx
