// Delta kernel
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "DeltaKernel.h"

namespace SX
{

namespace Imaging
{

ConvolutionKernel* DeltaKernel::create()
{
	return new DeltaKernel();
}

DeltaKernel::DeltaKernel()
{
    // default values
    _params["rows"] = 15;
    _params["cols"] = 15;
}

DeltaKernel::DeltaKernel(const SX::Imaging::ConvolutionKernel::ParameterMap &params)
{
    _params = params;

    // load default values if necessary
    if ( _params["rows"] <= 0)
        _params["rows"] = 15;

    if ( _params["cols"] <= 0)
        _params["cols"] = 15;
}

DeltaKernel::~DeltaKernel()
{
}

const char *DeltaKernel::getName()
{
    return "Delta";
}

void DeltaKernel::update()
{
    int rows, cols;

    // get necessary parameters
    rows = static_cast<int>(_params["rows"]);
    cols = static_cast<int>(_params["cols"]);

    // sanity checks
    if ( rows < 0 || cols < 0 )
        throw std::runtime_error("DeltaKernel::update() called with invalid parameters");

    // set kernel equal to kronecker delta
    _kernel = SX::Types::RealMatrix::Zero(rows, cols);
    _kernel(0,0) = 1.0;
}


} // Imaging

} // SX
