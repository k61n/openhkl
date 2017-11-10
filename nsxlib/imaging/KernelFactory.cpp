#include "AnnularKernel.h"
#include "ConstantKernel.h"
#include "DeltaKernel.h"
#include "KernelFactory.h"

namespace nsx {

KernelFactory::KernelFactory()
{
    registerCallback("annular",&AnnularKernel::create);
    registerCallback("constant",&ConstantKernel::create);
    registerCallback("delta",&DeltaKernel::create);
}

KernelFactory::~KernelFactory()
{
}

} // end namespace nsx

