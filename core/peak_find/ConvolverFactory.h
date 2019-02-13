#pragma once

#include <functional>

#include "Convolver.h"

namespace nsx {

//! \brief Factory class for the different image filters.
class ConvolverFactory {

public:
    using callback = std::function<Convolver*(const std::map<std::string,double>&)>;

    ConvolverFactory();

    Convolver* create(const std::string& convolver_type, const std::map<std::string,double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~ConvolverFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
