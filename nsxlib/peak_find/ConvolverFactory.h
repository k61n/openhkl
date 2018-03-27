#pragma once

#include <functional>
#include <memory>

#include "Convolver.h"
#include "PeakFindTypes.h"

namespace nsx {

class ConvolverFactory {

public:
    using callback = std::function<sptrConvolver(const std::map<std::string,double>&)>;

    ConvolverFactory();

    sptrConvolver create(const std::string& convolver_type, const std::map<std::string,double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~ConvolverFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
