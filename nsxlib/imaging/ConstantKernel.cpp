// Annular kernel
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "ConstantKernel.h"


namespace SX
{

namespace Imaging
{

ConvolutionKernel* ConstantKernel::create(int nrows, int ncols)
{
	return new ConstantKernel(nrows, ncols);
}

ConstantKernel::ConstantKernel(int nrows, int ncols) : ConvolutionKernel(nrows,ncols)
{
}

ConstantKernel::ConstantKernel(int nrows, int ncols, const SX::Imaging::ConvolutionKernel::ParameterMap &params)
: ConvolutionKernel(nrows,ncols,params)
{
}

ConstantKernel::~ConstantKernel()
{
}

const char *ConstantKernel::getName()
{
    return "Constant";
}

void SX::Imaging::ConstantKernel::update()
{
    int rows, cols;

    // get necessary parameters
    rows = _kernel.rows();
    cols = _kernel.cols();

    // sanity checks
    if ( rows < 0 || cols < 0 )
        throw std::runtime_error("DeltaKernel::update() called with invalid parameters");

    double value = 1.0 / (rows * cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            _kernel(i,j) = value;
        }
    }
}


} // Imaging

} // SX
