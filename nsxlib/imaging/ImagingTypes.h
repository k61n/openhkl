#ifndef NSXLIB_IMAGINGTYPES_H
#define NSXLIB_IMAGINGTYPES_H

#include <memory>

namespace nsx {

class ConvolutionKernel;
class Convolver;

using sptrConvolutionKernel = std::shared_ptr<ConvolutionKernel>;
using sptrConvolver = std::shared_ptr<Convolver>;

} // end namespace nsx

#endif // NSXLIB_IMAGINGTYPES_H
