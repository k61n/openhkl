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

namespace SX {
namespace Imaging {

class KernelFactory :
        public SX::Kernel::Factory<ConvolutionKernel,std::string,int,int>,
        public SX::Kernel::Singleton<KernelFactory,SX::Kernel::Constructor,SX::Kernel::Destructor>
{
public:
    KernelFactory();
    virtual ~KernelFactory();
};

} // namespace Imaging
} // namespace SX

#endif /* NSXLIB_IMAGING_KERNELFACTORY_H_ */
