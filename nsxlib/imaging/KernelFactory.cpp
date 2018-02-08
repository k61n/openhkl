#include "AnnularKernel.h"
#include "ConvolutionKernel.h"
#include "ConstantKernel.h"
#include "DeltaKernel.h"
#include "KernelFactory.h"

namespace nsx {

template <typename T>
sptrConvolutionKernel create_kernel(int nrows, int ncols)
{
    return std::make_shared<T>(nrows,ncols);
}

KernelFactory::KernelFactory(): _callbacks()
{
    _callbacks["annular"] = &create_kernel<AnnularKernel>;
    _callbacks["constant"] = &create_kernel<ConstantKernel>;
    _callbacks["delta"] = &create_kernel<DeltaKernel>;
}

sptrConvolutionKernel KernelFactory::create(const std::string& kernel_name, int nrows, int ncols) const
{
    const auto it = _callbacks.find(kernel_name);

    // could not find key
    if (it == _callbacks.end()) {
        throw std::runtime_error("could not find given extension in map of callbacks");
    }

    return (it->second)(nrows,ncols);
}

const std::map<std::string,KernelFactory::callback>& KernelFactory::callbacks() const
{
    return _callbacks;
}

} // end namespace nsx

