#include "AnnularKernel.h"
#include "ConvolutionKernel.h"
#include "ConstantKernel.h"
#include "DeltaKernel.h"
#include "KernelFactory.h"

namespace nsx {

template <typename T>
sptrConvolutionKernel create_kernel(const std::map<std::string,double>& parameters)
{
    return std::make_shared<T>(parameters);
}

KernelFactory::KernelFactory(): _callbacks()
{
    _callbacks["annular"] = &create_kernel<AnnularKernel>;
    _callbacks["constant"] = &create_kernel<ConstantKernel>;
    _callbacks["delta"] = &create_kernel<DeltaKernel>;
}

std::shared_ptr<ConvolutionKernel> KernelFactory::create(const std::string& kernel_type, const std::map<std::string,double>& parameters) const
{
    const auto it = _callbacks.find(kernel_type);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error("could not find given extension in map of callbacks");
    }

    return (it->second)(parameters);
}

const std::map<std::string,KernelFactory::callback>& KernelFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx

