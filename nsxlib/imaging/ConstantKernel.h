// Constant kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de


// Kernel description: this kernel is constant with the value 1 / (rows*cols).
// Convolving with this kernel has the effect of taking the average value of the frame.

#pragma once

#include "ConvolutionKernel.h"
#include "ImagingTypes.h"

namespace nsx {

class ConstantKernel : public ConvolutionKernel {

public:

    static ConvolutionKernel* create(int nrows, int ncols);

public:

    ConstantKernel(int nrows, int ncols);

    ConstantKernel(int nrows, int ncols, const ConvolutionKernelParameters& params);

    const char* name() override;

    virtual ~ConstantKernel();

private:

    void update() override;

};

} // end namespace nsx
