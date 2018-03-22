// Constant kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de


// Kernel description: this kernel is constant with the value 1 / (rows*cols).
// Convolving with this kernel has the effect of taking the average value of the frame.

#pragma once

#include "ConvolutionKernel.h"

namespace nsx {

class ConstantKernel : public ConvolutionKernel {

public:

    ConstantKernel();

    ConstantKernel(const std::map<std::string,double>& parameters);

    const char* name() const override;

    virtual ~ConstantKernel();

private:

    RealMatrix _matrix(int nrows, int ncols) const override;

};

} // end namespace nsx
