#include "DeltaConvolver.h"

namespace nsx {

DeltaConvolver::DeltaConvolver(const std::map<std::string,double>& parameters)
: DeltaConvolver()
{
}

DeltaConvolver::~DeltaConvolver()
{
}

const char* DeltaConvolver::name() const
{
    return "delta";
}

RealMatrix DeltaConvolver::_matrix(int nrows, int ncols) const
{
    RealMatrix kernel = RealMatrix::Zero(nrows, ncols);
    kernel(0,0) = 1.0;

    return kernel;
}

} // end namespace nsx

