// Constant kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de


// Kernel description: this kernel is constant with the value 1 / (rows*cols).
// Convolving with this kernel has the effect of taking the average value of the frame.


#ifndef NSXTOOL_INCLUDE_CONSTANTKERNEL_H_
#define NSXTOOL_INCLUDE_CONSTANTKERNEL_H_

#include "ConvolutionKernel.h"

namespace SX
{

namespace Imaging
{

class ConstantKernel : public ConvolutionKernel
{
public:
	static ConvolutionKernel* create(int nrows, int ncols);

public:
    ConstantKernel(int nrows, int ncols);
    ConstantKernel(int nrows, int ncols, const ConvolutionKernel::ParameterMap& params);

    const char* getName() override;

    virtual ~ConstantKernel();

private:
    void update() override;
};

} // Imaging
} // SX

#endif // NSXTOOL_INCLUDED_ANNULARKERNEL_H_
