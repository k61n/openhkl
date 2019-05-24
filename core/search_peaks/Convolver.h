#pragma once

#include <map>
#include <string>
#include <utility>

#include "MathematicsTypes.h"

namespace nsx {

class Convolver {

public:
    Convolver();

    Convolver(const std::map<std::string, double>& parameters);

    Convolver(const Convolver& other) = default;

    Convolver& operator=(const Convolver& other) = default;

    virtual ~Convolver() = 0;

    virtual Convolver* clone() const = 0;

    // Non-const getter for kernel parameter
    std::map<std::string, double>& parameters();

    // Const getter for kernel parameter
    const std::map<std::string, double>& parameters() const;

    void setParameters(const std::map<std::string, double>& parameters);

    //! Convolve an image
    virtual RealMatrix convolve(const RealMatrix& image) = 0;

    virtual std::pair<size_t, size_t> kernelSize() const = 0;

protected:
    std::map<std::string, double> _parameters;
};

} // end namespace nsx
