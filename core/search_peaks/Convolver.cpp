#include "Convolver.h"

namespace nsx {

using parameters_map = std::map<std::string, double>;

Convolver::Convolver() : _parameters() {}

Convolver::Convolver(const parameters_map& parameters)
{
    _parameters = parameters;
}

Convolver::~Convolver() {}

std::map<std::string, double>& Convolver::parameters()
{
    return _parameters;
}

const std::map<std::string, double>& Convolver::parameters() const
{
    return _parameters;
}

void Convolver::setParameters(const std::map<std::string, double>& parameters)
{
    for (auto p : parameters) {
        auto it = _parameters.find(p.first);
        if (it != _parameters.end()) {
            it->second = p.second;
        }
    }
}

} // end namespace nsx
