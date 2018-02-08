#pragma once

#include <functional>

#include "ImagingTypes.h"

namespace nsx {

class KernelFactory {

public:
    using callback = std::function<sptrConvolutionKernel(int,int)>;

    KernelFactory();

    sptrConvolutionKernel create(const std::string& kernel_name, int nrows, int ncols) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~KernelFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
