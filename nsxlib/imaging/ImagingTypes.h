#ifndef NSXLIB_IMAGINGTYPES_H
#define NSXLIB_IMAGINGTYPES_H

#include <map>
#include <memory>

namespace nsx {

class ConvolutionKernel;
class Convolver;

using sptrConvolutionKernel = std::shared_ptr<ConvolutionKernel>;
using sptrConvolver = std::shared_ptr<Convolver>;

using ConvolutionKernelParameters = std::map<std::string, double>;

} // end namespace nsx

#endif // NSXLIB_IMAGINGTYPES_H
