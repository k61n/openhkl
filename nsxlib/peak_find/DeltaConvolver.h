#pragma once

#include "Convolver.h"

namespace nsx {

class DeltaConvolver : public Convolver {

public:

    DeltaConvolver()=default;

    DeltaConvolver(const std::map<std::string,double>& parameters);

    virtual ~DeltaConvolver();

    const char* name() const override;

private:

    RealMatrix _matrix(int nrows, int ncols) const override;

};

} // end namespace nsx
