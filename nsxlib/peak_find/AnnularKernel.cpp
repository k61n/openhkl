// Annular kernel
// author: Jonathan Fisher
// Forschungszentrum Jülich GmbH
// j.fisher@fz-juelich.de

#include <math.h>

#include "AnnularKernel.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

// M_PI is often defined but not standard
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884
#endif

namespace nsx {

AnnularKernel::AnnularKernel()
: ConvolutionKernel()
{
    // default values
    _parameters["r1"] = 5;
    _parameters["r2"] = 10;
    _parameters["r3"] = 15;
}

AnnularKernel::AnnularKernel(const std::map<std::string,double>& parameters)
: AnnularKernel()
{
    setParameters(parameters);
}

AnnularKernel::~AnnularKernel()
{
}

const char* AnnularKernel::name() const
{
    return "annular";
}

RealMatrix AnnularKernel::_matrix(int nrows, int ncols) const
{
    int r1, r2, r3;

    r1 = static_cast<int>(_parameters.at("r1"));
    r2 = static_cast<int>(_parameters.at("r2"));
    r3 = static_cast<int>(_parameters.at("r3"));

    RealMatrix inner = RealMatrix::Zero(nrows, ncols);
    RealMatrix outer = RealMatrix::Zero(nrows, ncols);

    // sanity checks
    if (r1 < 0 || r2 < r1 || r3 < r2) {
        throw std::runtime_error("Annular kernel called with invalid parameters");
    }

    for (int i = 0; i < nrows; ++i) {
        for (int j = 0; j < ncols; ++j) {
            // shift so that (0,0) = (rows, cols) = (rows, 0) = (0, cols) is the center of the kernel
            double x = j > ncols/2 ? ncols-j : j;
            double y = i > nrows/2 ? nrows-i : i;

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

    RealMatrix kernel = inner - outer;

    return kernel;
}

} // end namespace nsx

