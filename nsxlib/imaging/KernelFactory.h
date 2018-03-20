#pragma once

#include <functional>
#include <memory>

#include "ConvolutionKernel.h"

namespace nsx {

class KernelFactory {

public:
    using callback = std::function<sptrConvolutionKernel(const std::map<std::string,double>&)>;

    KernelFactory();

    std::shared_ptr<ConvolutionKernel> create(const std::string& kernel_type, const std::map<std::string,double>& parameters) const;

    //! return the keys of the object currently registered in the factory
    const std::map<std::string,callback>& callbacks() const;

    ~KernelFactory()=default;

private:
    std::map<std::string, callback> _callbacks;
};

} // end namespace nsx
