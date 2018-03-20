#include "ConstantKernel.h"

namespace nsx {

ConstantKernel::ConstantKernel(const std::map<std::string,double>& parameters)
: ConstantKernel()
{
}

ConstantKernel::~ConstantKernel()
{
}

const char* ConstantKernel::name() const
{
    return "Constant";
}

RealMatrix ConstantKernel::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel(nrows,ncols);

    double value = 1.0 / (nrows * ncols);

    for (int i = 0; i < nrows; ++i) {
        for (int j = 0; j < ncols; ++j) {
            kernel(i,j) = value;
        }
    }

    return kernel;
}

} // end namespace nsx

