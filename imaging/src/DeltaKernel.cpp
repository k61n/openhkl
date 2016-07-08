// Delta kernel
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "DeltaKernel.h"

namespace SX {

namespace Imaging {


DeltaKernel::DeltaKernel()
{
    // default values
    _params["rows"] = 15;
    _params["cols"] = 15;
    _params["r1"] = 5;
    _params["r2"] = 10;
    _params["r3"] = 15;
}

SX::Imaging::DeltaKernel::DeltaKernel(const SX::Imaging::ConvolutionKernel::ParameterMap &params)
{
    _params = params;

    // load default values if necessary
    if ( _params["rows"] >= 0)
        _params["rows"] = 15;

    if ( _params["cols"] >= 0)
        _params["cols"] = 15;
}

const char *DeltaKernel::getName()
{
    return "Delta";
}

void SX::Imaging::DeltaKernel::update()
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
