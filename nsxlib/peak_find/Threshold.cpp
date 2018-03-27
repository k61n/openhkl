#include "Threshold.h"

namespace nsx {

Threshold::Threshold()
: _parameters()
{
}

Threshold::~Threshold()
{
}

std::map<std::string,double>& Threshold::parameters()
{
    return _parameters;
}

const std::map<std::string,double>& Threshold::parameters() const
{
    return _parameters;
}

void Threshold::setParameters(const std::map<std::string,double>& parameters)
{
    for (auto p : parameters) {
        auto it = _parameters.find(p.first);
        if (it != _parameters.end()) {
            it->second = p.second;
        }
    }
}

} // end namespace nsx
