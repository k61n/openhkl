// Annular kernel
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "ConstantKernel.h"


namespace SX
{

namespace Imaging
{

ConvolutionKernel* ConstantKernel::create()
{
	return new ConstantKernel();
}

ConstantKernel::ConstantKernel()
{
    // default values
    _params["rows"] = 15;
    _params["cols"] = 15;
}

ConstantKernel::ConstantKernel(const SX::Imaging::ConvolutionKernel::ParameterMap &params)
{
    _params = params;

    // load default values if necessary
    if ( _params["rows"] <= 0)
        _params["rows"] = 15;

    if ( _params["cols"] <= 0)
        _params["cols"] = 15;
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
    rows = static_cast<int>(_params["rows"]);
    cols = static_cast<int>(_params["cols"]);

    

    // sanity checks
    if ( rows < 0 || cols < 0 )
        throw std::runtime_error("ConstantKernel::update() called with invalid parameters");

    // sanity checks passed, now we proceed
    _kernel.resize(rows, cols);

    double value = 1.0 / (rows * cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            _kernel(i,j) = value;
        }
    }
}


} // Imaging

} // SX
