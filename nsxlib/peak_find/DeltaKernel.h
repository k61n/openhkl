#pragma once

#include "ConvolutionKernel.h"

namespace nsx {

class DeltaKernel : public ConvolutionKernel {

public:

    DeltaKernel()=default;

    DeltaKernel(const std::map<std::string,double>& parameters);

    virtual ~DeltaKernel();

    const char* name() const override;

private:

    RealMatrix _matrix(int nrows, int ncols) const override;

};

} // end namespace nsx
