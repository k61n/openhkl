// Convolver description: this kernel is constant with the value 1 / (rows*cols).
// Convolving with this kernel has the effect of taking the average value of the frame.

#pragma once

#include "Convolver.h"

namespace nsx {

class ConstantConvolver : public Convolver {

public:

    ConstantConvolver();

    ConstantConvolver(const std::map<std::string,double>& parameters);

    const char* name() const override;

    virtual ~ConstantConvolver();

private:

    RealMatrix _matrix(int nrows, int ncols) const override;

};

} // end namespace nsx
