#include "DeltaKernel.h"

namespace nsx {

DeltaKernel::DeltaKernel(const std::map<std::string,double>& parameters)
: DeltaKernel()
{
}

DeltaKernel::~DeltaKernel()
{
}

const char* DeltaKernel::name() const
{
    return "delta";
}

RealMatrix DeltaKernel::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    kernel(0,0) = 1.0;

    return kernel;
}

} // end namespace nsx

