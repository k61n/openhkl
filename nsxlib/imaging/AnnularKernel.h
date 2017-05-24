// Annular kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de


// Kernel description: the kernel is non-zero in a circular and annular region, as follows

// [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]
// [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]
// [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]
// [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2]
// [0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0]
// [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0]
// [0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0]
// [0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0]
// [0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0]
// [0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0]

// In region 1, pixels are assigned constant positive value
// In region 2, pixels are assigned a constant negative value.
// The values are chosen so that the integral over region 1 is equal to +1,
// and the integral over region 2 is equal to -1
//
// The motivation is as follows: if a peak is contained inside region 1, then
// the convolution with region 1 computes the average intensity of the peak,
// and the convolution with region 2 computes (minus) the average background
// The the total convolution computes the average intensity minus the average background.
// This make the kernel effective for peak-finding in images which have a non-homogeneous
// background.


#ifndef NSXLIB_ANNULARKERNEL_H
#define NSXLIB_ANNULARKERNEL_H

#include "../imaging/ImagingTypes.h"
#include "ConvolutionKernel.h"

namespace nsx {

class AnnularKernel : public ConvolutionKernel {

public:

    static ConvolutionKernel* create(int nrows, int ncols);

public:

    AnnularKernel(int nrows, int ncols);

    AnnularKernel(int nrows, int ncols, const ConvolutionKernelParameters& params);

    virtual ~AnnularKernel();

    const char* getName() override;

private:

    void update() override;
};

} // end namespace nsx

#endif // NSXLIB_ANNULARKERNEL_H
