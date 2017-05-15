// Annular kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
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

namespace nsx {
namespace Imaging {

ConvolutionKernel* AnnularKernel::create(int nrows, int ncols)
{
	return new AnnularKernel(nrows,ncols);
}

AnnularKernel::AnnularKernel(int nrows, int ncols) : ConvolutionKernel(nrows,ncols)
{
    // default values
    _params["r1"] = 5;
    _params["r2"] = 10;
    _params["r3"] = 15;
}

nsx::Imaging::AnnularKernel::AnnularKernel(int nrows, int ncols, const nsx::Imaging::ConvolutionKernel::ParameterMap &params)
: ConvolutionKernel(nrows,ncols,params)
{
    // load default values if necessary
    if ( _params["r1"] <= 0) {
        _params["r1"] = 3;
    }
    if ( _params["r2"] <= 0) {
        _params["r2"] = 6;
    }
    if ( _params["r3"] <= 0) {
        _params["r3"] = 10;
    }
}

AnnularKernel::~AnnularKernel()
{
}

const char *AnnularKernel::getName()
{
    return "Annular";
}

void nsx::Imaging::AnnularKernel::update()
{
    int rows, cols, r1, r2, r3;

    // get necessary parameters
    rows = _kernel.rows();
    cols = _kernel.cols();
    r1 = static_cast<int>(_params["r1"]);
    r2 = static_cast<int>(_params["r2"]);
    r3 = static_cast<int>(_params["r3"]);

    nsx::Types::RealMatrix inner = nsx::Types::RealMatrix::Zero(rows, cols);
    nsx::Types::RealMatrix outer = nsx::Types::RealMatrix::Zero(rows, cols);

    // sanity checks
    if (rows < 0 || cols < 0 || r1 < 0 || r2 < r1 || r3 < r2) {
        throw std::runtime_error("AnnularKernel::update() called with invalid parameters");
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // shift so that (0,0) = (rows, cols) = (rows, 0) = (0, cols) is the center of the kernel
            double x = j > cols/2 ? cols-j : j;
            double y = i > rows/2 ? rows-i : i;

            double dist2 = x*x + y*y;

            if (dist2 > r3*r3) {
                continue;
            }
            else if (dist2 <= r1*r1) {
                inner(i, j) = 1.0;
            }
            else if (dist2 > r2*r2) {
                outer(i, j) = 1.0;
            }
        }
    }

    inner /= inner.sum();
    outer /= outer.sum();
    _kernel = inner - outer;
}

} // Imaging
} // SX
