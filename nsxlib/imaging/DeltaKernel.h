// Delta kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de


// Kronecker delta kernel. Used for testing only

#ifndef NSXLIB_DELTAKERNEL_H
#define NSXLIB_DELTAKERNEL_H

#include "../imaging/ImagingTypes.h"
#include "ConvolutionKernel.h"

namespace nsx {

class DeltaKernel : public ConvolutionKernel {

public:

    static ConvolutionKernel* create(int nrows, int ncols);

public:

    DeltaKernel(int nrows, int ncols);

    DeltaKernel(int nrows, int ncols, const ConvolutionKernelParameters& params);

    virtual ~DeltaKernel();

    const char* getName() override;

private:

    void update() override;

};

} // end namespace nsx

#endif // NSXLIB_DELTAKERNEL_H
