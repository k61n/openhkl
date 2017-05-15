/*
 * KernelFactory.h
 *
 *  Created on: Nov 14, 2016
 *      Author: pellegrini
 */

#ifndef NSXLIB_IMAGING_KERNELFACTORY_H_
#define NSXLIB_IMAGING_KERNELFACTORY_H_

#include "ConvolutionKernel.h"
#include "../kernel/Factory.h"
#include "../kernel/Singleton.h"

namespace nsx {
namespace Imaging {

class KernelFactory :
        public nsx::Kernel::Factory<ConvolutionKernel,std::string,int,int>,
        public nsx::Kernel::Singleton<KernelFactory,nsx::Kernel::Constructor,nsx::Kernel::Destructor>
{
public:
    KernelFactory();
    virtual ~KernelFactory();
};

} // namespace Imaging
} // end namespace nsx

#endif /* NSXLIB_IMAGING_KERNELFACTORY_H_ */
