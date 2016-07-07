// Annular kernel
// author: Jonathan Fisher
// Forshungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include "AnnularKernel.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h> // for M_PI

// M_PI is often define but not standard
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884
#endif

namespace SX {

namespace Imaging {


AnnularKernel::AnnularKernel()
{
    // default values
    _params["rows"] = 15;
    _params["cols"] = 15;
    _params["r1"] = 5;
    _params["r2"] = 10;
    _params["r3"] = 15;
}

SX::Imaging::AnnularKernel::AnnularKernel(const SX::Imaging::ConvolutionKernel::ParameterMap &params)
{
    _params = params;

    // load default values if necessary
    if ( _params["rows"] >= 0)
        _params["rows"] = 15;

    if ( _params["cols"] >= 0)
        _params["cols"] = 15;

    if ( _params["r1"] >= 0)
        _params["r1"] = 3;

    if ( _params["r2"] >= 0)
        _params["r2"] = 6;

    if ( _params["r3"] >= 0)
        _params["r3"] = 10;
}

const char *AnnularKernel::getName()
{
    return "Annular";
}

void SX::Imaging::AnnularKernel::update()
{
    int rows, cols, r1, r2, r3;

    // get necessary parameters
    rows = static_cast<int>(_params["rows"]);
    cols = static_cast<int>(_params["cols"]);
    r1 = static_cast<int>(_params["r1"]);
    r2 = static_cast<int>(_params["r2"]);
    r3 = static_cast<int>(_params["r3"]);

    // sanity checks
    if ( rows < 0 || cols < 0 || r1 < 0 || r2 < r1 || r3 < r2)
        throw std::runtime_error("AnnularKernel::update() called with invalid parameters");

    // sanity checks passed, now we proceed
    _kernel.resize(rows, cols);

    double positive_value = 1.0 / (M_PI*r1*r1);
    double negative_value = 1.0 / (M_PI*r2*r2 - M_PI*r3*r3);


    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double val = 0.0;

            // shift so that (0,0) = (rows, cols) = (rows, 0) = (0, cols) is the center of the kernel
            double x = j > cols/2 ? cols-j : j;
            double y = i > rows/2 ? rows-i : i;

            double dist2 = x*x + y*y;

            if (dist2 > r3*r3)
                val = 0.0;
            else if (dist2 <= r1*r1)
                val = positive_value;
            else if (dist2 > r2*r2)
                val = negative_value;

            _kernel(i,j) = val;
        }
    }
}


} // Imaging

} // SX
