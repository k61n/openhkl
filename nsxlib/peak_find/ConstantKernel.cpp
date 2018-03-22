#include <iostream>
#include "ConstantKernel.h"

namespace nsx {

ConstantKernel::ConstantKernel()
{
    _parameters["box_size"] = 3;
}

ConstantKernel::ConstantKernel(const std::map<std::string,double>& parameters)
: ConstantKernel()
{
    setParameters(parameters);
}

ConstantKernel::~ConstantKernel()
{
}

const char* ConstantKernel::name() const
{
    return "constant";
}

RealMatrix ConstantKernel::_matrix(int nrows, int ncols) const
{
    const int box_size = static_cast<int>(_parameters.at("box_size"));

    // sanity checks
    if (box_size <= 0) {
        throw std::runtime_error("Constant kernel called with invalid parameters");
    }

    const double norm_factor = static_cast<double>(box_size*box_size);

    RealMatrix kernel = RealMatrix::Zero(nrows,ncols);

    const int half = box_size / 2;
    const int offset = box_size % 2 ? 0 : 1;
    for (int i = -half + offset; i <= half; ++i) {
        const int row = (i + nrows - offset)%nrows;
        for (int j = -half + offset; j <= half; ++j) {
            const int col = (j + ncols - offset)%ncols;
            kernel(row,col) = 1.0/norm_factor;
        }
    }

    return kernel;
}

} // end namespace nsx

