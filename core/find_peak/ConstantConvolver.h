// Convolver description: this kernel is constant with the value 1 / (rows*cols).
// Convolving with this kernel has the effect of taking the average value of the frame.

#pragma once

#include "AtomicConvolver.h"

namespace nsx {

class ConstantConvolver : public AtomicConvolver {

public:
    ConstantConvolver();

    ConstantConvolver(const ConstantConvolver& other) = default;

    ConstantConvolver(const std::map<std::string, double>& parameters);

    ~ConstantConvolver() = default;

    ConstantConvolver& operator=(const ConstantConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // end namespace nsx
