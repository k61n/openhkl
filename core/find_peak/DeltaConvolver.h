#pragma once

#include "AtomicConvolver.h"

namespace nsx {

class DeltaConvolver : public AtomicConvolver {

public:
    DeltaConvolver();

    DeltaConvolver(const DeltaConvolver& other) = default;

    DeltaConvolver(const std::map<std::string, double>& parameters);

    ~DeltaConvolver() = default;

    DeltaConvolver& operator=(const DeltaConvolver& other) = default;

    Convolver* clone() const override;

    virtual std::pair<size_t, size_t> kernelSize() const override;

private:
    RealMatrix _matrix(int nrows, int ncols) const override;
};

} // end namespace nsx
