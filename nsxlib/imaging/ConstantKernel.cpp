// Annular kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "ConstantKernel.h"

namespace nsx {

ConvolutionKernel* ConstantKernel::create(int nrows, int ncols)
{
	return new ConstantKernel(nrows, ncols);
}

ConstantKernel::ConstantKernel(int nrows, int ncols) : ConvolutionKernel(nrows,ncols)
{
}

ConstantKernel::ConstantKernel(int nrows, int ncols, const ConvolutionKernelParameters &params)
: ConvolutionKernel(nrows,ncols,params)
{
}

ConstantKernel::~ConstantKernel()
{
}

const char* ConstantKernel::name()
{
    return "Constant";
}

void ConstantKernel::update()
{
    int rows, cols;

    // get necessary parameters
    rows = _kernel.rows();
    cols = _kernel.cols();

    // sanity checks
    if ( rows < 0 || cols < 0 ) {
        throw std::runtime_error("Deltaupdate() called with invalid parameters");
    }

    double value = 1.0 / (rows * cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            _kernel(i,j) = value;
        }
    }
}

} // end namespace nsx

