#pragma once

#include "ConvolutionKernel.h"
#include "ImagingTypes.h"

namespace nsx {

class DeltaKernel : public ConvolutionKernel {

public:

    static ConvolutionKernel* create(int nrows, int ncols);

public:

    DeltaKernel(int nrows, int ncols);

    DeltaKernel(int nrows, int ncols, const ConvolutionKernelParameters& params);

    virtual ~DeltaKernel();

    const char* name() override;

private:

    void update() override;

};

} // end namespace nsx
